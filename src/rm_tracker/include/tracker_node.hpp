
// ROS
#include <message_filters/subscriber.h>
#include <tf2_ros/buffer.h>
#include <tf2_ros/create_timer_ros.h>
#include <tf2_ros/message_filter.h>
#include <tf2_ros/transform_listener.h>

#include <rclcpp/rclcpp.hpp>
#include <std_srvs/srv/trigger.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <visualization_msgs/msg/marker_array.hpp>

// STD
#include <memory>
#include <string>
#include <vector>

#include"rm_msg_interfaces/msg/armor.hpp"
#include"rm_msg_interfaces/msg/armors.hpp"

#include <tracker.hpp>


namespace rm_auto_aim
{
class TrackerNode : public rclcpp::Node
{
public:
  TrackerNode();
 
  std::shared_ptr<Tracker> tracker_;
  std::shared_ptr<tf2_ros::Buffer> tf2_buffer_;
  std::shared_ptr<tf2_ros::TransformListener> tf2_listener_;
  
  std::shared_ptr<tf2_ros::MessageFilter<rm_msg_interfaces::msg::Armors>> tf2_filter_;
  std::string target_frame_;
  std::shared_ptr<tf2_ros::CreateTimerROS> timer_;
  
 message_filters::Subscriber<rm_msg_interfaces::msg::Armors> armor_sub_;

  void ArmorCallback(const rm_msg_interfaces::msg::Armors::SharedPtr& msg);
  std::shared_ptr<Tracker> initTracker();

};
}