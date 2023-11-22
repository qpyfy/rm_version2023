#include <cv_bridge/cv_bridge.h>
#include <rmw/qos_profiles.h>
#include <tf2/LinearMath/Matrix3x3.h>
#include <tf2/convert.h>

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <image_transport/image_transport.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <rclcpp/duration.hpp>
#include <rclcpp/qos.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

#include <detector_node.hpp>
#include "rm_msg_interfaces/msg/armor.hpp"
#include "rm_msg_interfaces/msg/armors.hpp"

// STD
#include <algorithm>
#include <map>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace rm_auto_aim
{

   ArmorDetectorNode::ArmorDetectorNode(const rclcpp::NodeOptions &options)
       : rclcpp::Node("detector_node", options)
   {
      RCLCPP_INFO(this->get_logger(), "Dector_Node start");
      // 1.初始化
      this->detector_ = this->initDetector();
      this->number_classifier_ = this->initNumberClassifier();
      this->pnp_solver_ = this->initPnpSolver();
      debug = this->declare_parameter("Debug", true);
      if (debug == true)
      {
         this->initDebugPuilsher();
      }

      // 2.接受图像信息
      img_sub_ = this->create_subscription<sensor_msgs::msg::Image>("/image_raw",
                                                                    rclcpp::SensorDataQoS(),
                                                                    std::bind(&ArmorDetectorNode::ImgCallback, this, std::placeholders::_1));

      armors_pub_ = this->create_publisher<rm_msg_interfaces::msg::Armors>("/armors", 10);
   }

   void ArmorDetectorNode::ImgCallback(const sensor_msgs::msg::Image::ConstSharedPtr img_msg)
   {
      // 1.转成opencv类型
      RCLCPP_INFO(this->get_logger(), "ImgCallback");
      img_ = cv_bridge::toCvShare(img_msg, "rgb8")->image;
      // 2.判断装甲板
      this->GetArmors();

      // 3,判断装甲板的三维坐标
      for (auto &armor : armors_)
      {
         cv::Mat rvec, tvec;
         if (this->pnp_solver_->solvePnP(armor, rvec, tvec))
         {
            armor.rvec = rvec;
            armor.tvec = tvec;
         }
         else
         {
            RCLCPP_INFO(this->get_logger(), "solvePnP failed");
         }
      }

      // 3.发布装甲板信息
      if (armors_.size() > 0)
         ArmorPubilsh(armors_);
      else
      {
         RCLCPP_INFO(this->get_logger(), "armors_.size()=0");
      }
      // 4.发布调试信息

      if (debug == true)
      {
         drawLights();
         PuilshDebugImg();
      }
   }

   std::vector<Armor> ArmorDetectorNode::GetArmors()
   {
      // 1.预处理
      PreProcess();
      // 2.找灯条
      lights_ = this->detector_->findLights(bin_img_);
      // 3.灯条匹配
      armors_ = this->detector_->matchArmor(lights_);

      if (armors_.size() > 0)
      {
         this->number_classifier_->extractNumbers(img_, armors_);
         this->number_classifier_->classifyNumbers(armors_);
      }
      return armors_;
   }
   /// @brief
   /// @param img
   /// @param bin_img
   /// @return
   void ArmorDetectorNode::PreProcess()
   {

      //    灰度化
      cv::cvtColor(img_, bin_img_, cv::COLOR_BGR2GRAY);
      //    二值化
      cv::threshold(bin_img_, bin_img_, detector_->binary_thres, 255, cv::THRESH_BINARY);
   }
   void ArmorDetectorNode::ArmorPubilsh(const std::vector<Armor> &armors)
   {
      armors_msg_.armors.clear();
      for (auto &armor : armors)
      {
         rm_msg_interfaces::msg::Armor armor_msg;
         armor_msg.type = armor.type;
         armor_msg.pose.position.x = armor.tvec.at<double>(0);
         armor_msg.pose.position.y = armor.tvec.at<double>(1);
         armor_msg.pose.position.z = armor.tvec.at<double>(2);
         armor_msg.distance_to_image_center = pnp_solver_->calculateDistanceToCenter(armor.center_point);
         ;
         armor_msg.number = armor.num;
         armors_msg_.armors.push_back(armor_msg);
      }
      armors_pub_->publish(armors_msg_);
   }
   /// @brief 初始化装甲板检测器
   /// @param
   /// @return
   std::unique_ptr<Detector> ArmorDetectorNode::initDetector()
   {
      std::unique_ptr<Detector> detector = std::make_unique<Detector>();
      detector->binary_thres = this->declare_parameter("binary_thres", 80);
      detector->light_max_angle = this->declare_parameter("light_max_angle", 40);
      detector->light_min_l_w_ratio = this->declare_parameter("light_min_l_w_ratio", 0.1);
      detector->light_max_l_w_ratio = this->declare_parameter("light_max_l_w_ratio", 0.4);
      detector->light_detector_color = this->declare_parameter("light_detector_color", Red);
      detector->armor_max_angle = this->declare_parameter("armor_max_angle", 35);
      detector->large_armor_max_center_disatance = this->declare_parameter("large_armor_max_center_disatance", 5.5);
      detector->small_armor_max_center_disatance = this->declare_parameter("small_armor_max_center_disatance", 3.2);
      detector->large_armor_min_center_disatance = this->declare_parameter("large_armor_min_center_disatance", 3.2);
      detector->small_armor_min_center_disatance = this->declare_parameter("small_armor_min_center_disatance", 0.8);
      RCLCPP_INFO(this->get_logger(), "Detector init");
      return detector;
   }

   std::unique_ptr<NumberClassifier> ArmorDetectorNode::initNumberClassifier()
   {
      std::unique_ptr<NumberClassifier> number_classifier = std::make_unique<NumberClassifier>();
      number_classifier->model_path = this->declare_parameter("svm_model_path", "src/rm_detector/model/mlp.onnx");
      RCLCPP_INFO(this->get_logger(), "NumberClassifier init");
      std::ifstream label_file("src/rm_detector/model/label.txt");
      number_classifier->threshold_ = this->declare_parameter("threshold", 0.8);
      number_classifier->ignore_classes_ = this->declare_parameter<std::vector<std::string>>("ignore_classes", {"negative"});

      std::string line;
      while (std::getline(label_file, line)) {
      number_classifier->class_names_.push_back(line);
            }
      number_classifier->net_ = cv::dnn::readNetFromONNX(number_classifier->model_path);

      return number_classifier;
   }

   std::unique_ptr<PnpSolver> ArmorDetectorNode::initPnpSolver()
   {
      std::unique_ptr<PnpSolver> pnp_solver = std::make_unique<PnpSolver>();
      pnp_solver->camera_matrix_ = (cv::Mat_<double>(3, 3) << 1807.12121, 0., 711.11997, 0.,
                                    1806.46896, 562.49495, 0., 0., 1.);
      pnp_solver->dist_coeffs_ = (cv::Mat_<double>(5, 1) << -0.078049, 0.158627, 0.000304, -0.000566, 0.000000);

      
      pnp_solver->LARGE_ARMOR_HEIGHT = this->declare_parameter("LARGE_ARMOR_HEIGHT", 55);
      pnp_solver->LARGE_ARMOR_WIDTH = this->declare_parameter("LARGE_ARMOR_WIDTH", 225);
      pnp_solver->SMALL_ARMOR_HEIGHT = this->declare_parameter("SMALL_ARMOR_HEIGHT", 55);
      pnp_solver->SMALL_ARMOR_WIDTH = this->declare_parameter("SMALL_ARMOR_WIDTH", 135);

      // Unit: m
      double small_half_y = pnp_solver->LARGE_ARMOR_HEIGHT / 2.0 / 1000.0;
      double small_half_z = pnp_solver->SMALL_ARMOR_HEIGHT / 2.0 / 1000.0;
      double large_half_y = pnp_solver->LARGE_ARMOR_WIDTH / 2.0 / 1000.0;
      double large_half_z = pnp_solver->LARGE_ARMOR_HEIGHT / 2.0 / 1000.0;

      // Start from bottom left in clockwise order
      // Model coordinate: x forward, y left, z up
      pnp_solver->small_armor_points_.emplace_back(cv::Point3f(0, small_half_y, -small_half_z));
      pnp_solver->small_armor_points_.emplace_back(cv::Point3f(0, small_half_y, small_half_z));
      pnp_solver->small_armor_points_.emplace_back(cv::Point3f(0, -small_half_y, small_half_z));
      pnp_solver->small_armor_points_.emplace_back(cv::Point3f(0, -small_half_y, -small_half_z));

      pnp_solver->large_armor_points_.emplace_back(cv::Point3f(0, large_half_y, -large_half_z));
      pnp_solver->large_armor_points_.emplace_back(cv::Point3f(0, large_half_y, large_half_z));
      pnp_solver->large_armor_points_.emplace_back(cv::Point3f(0, -large_half_y, large_half_z));
      pnp_solver->large_armor_points_.emplace_back(cv::Point3f(0, -large_half_y, -large_half_z));
      RCLCPP_INFO(this->get_logger(), "PnpSolver init");
      return pnp_solver;
   }

   void ArmorDetectorNode::initDebugPuilsher()
   {
      debug_img_pub_ = image_transport::create_publisher(this,"/debug_img");
      bin_img_pub_ = image_transport::create_publisher(this,"/bin_img");
      result_img_pub_ = image_transport::create_publisher(this,"/result_img");
      Armor_number_img_pub_ = image_transport::create_publisher(this,"/armor_number_img");
   }
   void ArmorDetectorNode::PuilshDebugImg()
   {
      debug_img_pub_.publish(
          *cv_bridge::CvImage(std_msgs::msg::Header(), "rgb8", img_).toImageMsg());
      std::shared_ptr<sensor_msgs::msg::Image> bin_img_msg = cv_bridge::CvImage(std_msgs::msg::Header(), "mono8", bin_img_).toImageMsg();
      bin_img_pub_.publish(*bin_img_msg);
      std::shared_ptr<sensor_msgs::msg::Image> result_img_msg = cv_bridge::CvImage(std_msgs::msg::Header(), "rgb8", result_img_).toImageMsg();
      result_img_pub_.publish(*result_img_msg);
      for(auto &armor:armors_){
      Armor_number_img_pub_.publish(*cv_bridge::CvImage(std_msgs::msg::Header(), "mono8", armor.number_img).toImageMsg());
      }
   }

   cv::Mat ArmorDetectorNode::drawLights()
   {
      result_img_ = img_;
      for (auto &light : lights_)
      {
         cv::Point2f vertices[4];
         light.rect.points(vertices);
         cv::line(result_img_,vertices[0],vertices[1], cv::Scalar(0, 255, 0), 2);
         cv::line(result_img_,vertices[1],vertices[2], cv::Scalar(0, 255, 0), 2);
         cv::line(result_img_,vertices[2],vertices[3], cv::Scalar(0, 255, 0), 2);
         cv::line(result_img_,vertices[3],vertices[0], cv::Scalar(0, 255, 0), 2);
      }

   for(auto &armor:armors_){
      cv::line(result_img_,armor.left_light.top_point,armor.left_light.bottom_point,cv::Scalar(0,255,0),1);
      cv::line(result_img_,armor.right_light.top_point,armor.right_light.bottom_point,cv::Scalar(0,255,0),1);
      cv::line(result_img_,armor.left_light.top_point,armor.right_light.top_point,cv::Scalar(0,255,0),1);
      cv::line(result_img_,armor.left_light.bottom_point,armor.right_light.bottom_point,cv::Scalar(0,255,0),1);
      cv::putText(result_img_,armor.num,armor.center_point,cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(0,255,0),2);
   }
      return result_img_;
   }
}
#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(rm_auto_aim::ArmorDetectorNode)
