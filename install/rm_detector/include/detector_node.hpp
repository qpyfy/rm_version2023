
#ifndef DETECTOR_NODE_HPP_
#define DETECTOR_NODE_HPP_
// ROS
#include <image_transport/image_transport.hpp>
#include <image_transport/publisher.hpp>
#include <image_transport/subscriber_filter.hpp>
#include <rclcpp/publisher.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/camera_info.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <visualization_msgs/msg/marker_array.hpp>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
// STD
#include <memory>
#include <string>
#include <vector>

#include <armor.hpp>
#include <detector.hpp>
#include <number_classifier.hpp>
#include <pnp_solver.hpp>
#include"rm_msg_interfaces/msg/armor.hpp"
#include"rm_msg_interfaces/msg/armors.hpp"



namespace rm_auto_aim{


#define Red 0
#define Blue 1


class ArmorDetectorNode:public rclcpp::Node
{
private:
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr img_sub_;
    rclcpp::Publisher<rm_msg_interfaces::msg::Armors>::SharedPtr armors_pub_;
    //debug
    image_transport::Publisher debug_img_pub_;
    image_transport::Publisher bin_img_pub_;
    image_transport::Publisher result_img_pub_;
    image_transport::Publisher Armor_number_img_pub_;

    std::unique_ptr<Detector> detector_;
    std::unique_ptr<NumberClassifier> number_classifier_;
    std::unique_ptr<PnpSolver> pnp_solver_;
    bool debug;
    cv::Mat img_;
    cv::Mat bin_img_;
    cv::Mat result_img_;

    std::vector<Light> lights_;
    std::vector<Armor> armors_;
    rm_msg_interfaces::msg::Armors armors_msg_;
    
    void ImgCallback(const sensor_msgs::msg::Image::ConstSharedPtr img_msg);
    std::vector<Armor> GetArmors();
    void PreProcess();
    void ArmorPubilsh(const std::vector<Armor> & armors);

    std::unique_ptr<Detector> initDetector();
    std::unique_ptr<NumberClassifier> initNumberClassifier();
    std::unique_ptr<PnpSolver> initPnpSolver();
    void initDebugPuilsher();
    cv::Mat drawLights();
    void PuilshDebugImg();

public:
    ArmorDetectorNode(const rclcpp::NodeOptions & options);
};


}

#endif