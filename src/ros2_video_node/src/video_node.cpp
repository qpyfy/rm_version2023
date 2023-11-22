#include <iostream>
#include <string>
// ROS
#include <image_transport/image_transport.hpp>
#include <rclcpp/logging.hpp>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp/utilities.hpp>
#include <sensor_msgs/msg/camera_info.hpp>
#include <sensor_msgs/msg/image.hpp>
// OpenCV
#include <cv_bridge/cv_bridge.h>
#include <opencv2/core/types.hpp>
#include <opencv2/opencv.hpp>

namespace ros2_video_node
{
   class VideoNode : public rclcpp::Node
   {
   public:
      explicit VideoNode(const rclcpp::NodeOptions &options) : Node("video_node", options)
      {
         RCLCPP_INFO(this->get_logger(), "Starting VideoNode!");

         video.open(this->declare_parameter("video_path", " "));
         while (rclcpp::ok())
         {
            if (video.read(image))
            {
               /* code */
               sensor_msgs::msg::Image image_msg_;
               cv_bridge::CvImage cv_image;
               cv_image.header.stamp = this->now();
               cv_image.header.frame_id = "video_optical_frame";
               cv_image.encoding = "bgr8";
               cv_image.image = image;
               image_msg_ = *cv_image.toImageMsg();
               video_pub_->publish(image_msg_);
            }
            else
            {
               // 如果视频播放完毕，则重新开始播放
               video.set(cv::CAP_PROP_POS_FRAMES, 0);
               // break;
               //  RCLCPP_INFO(this->get_logger(),"Waiting Image!");
               // video.open("src/ros2_video_node/ood_blue.mp4");
            }
         }
      }

   private:
      cv::VideoCapture video;
      cv::Mat image;
      rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr video_pub_ = this->create_publisher<sensor_msgs::msg::Image>("/image_raw", 10);
   };
}
#include "rclcpp_components/register_node_macro.hpp"

RCLCPP_COMPONENTS_REGISTER_NODE(ros2_video_node::VideoNode)
