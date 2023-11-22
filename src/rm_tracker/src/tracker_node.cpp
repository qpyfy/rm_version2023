#include <tracker_node.hpp>

#include <memory>
#include <vector>


enum{
LOST, 
FINDING, 
TRACKING,
};

namespace rm_auto_aim{

    TrackerNode::TrackerNode():Node("tracker_node")
    {
        RCLCPP_INFO(this->get_logger(), "Initializing tracker node");
        this->tracker_ = initTracker();


        //初始化
        this->timer_ = std::make_shared<tf2_ros::CreateTimerROS>(
            this->get_node_base_interface(),
            this->get_node_timers_interface());
        this->armor_sub_.subscribe(this, "armors");
        this->target_frame_ = this->declare_parameter("target_frame", "odom");
        this->tf2_buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
        this->tf2_buffer_->setCreateTimerInterface(this->timer_);
        this->tf2_filter_ = std::make_shared<tf2_ros::MessageFilter<rm_msg_interfaces::msg::Armors>>(
            armor_sub_,
            *this->tf2_buffer_,
            target_frame_,10,
            this->get_node_logging_interface(),
            this->get_node_clock_interface(),
            std::chrono::duration<int>(1)
        );
        this->tf2_filter_->registerCallback(&TrackerNode::ArmorCallback, this);
    }

    void TrackerNode::ArmorCallback(const rm_msg_interfaces::msg::Armors::SharedPtr& msg)
    {
        //坐标转换
        for( auto armor : msg->armors)
        {
        geometry_msgs::msg::PoseStamped pose;
        pose.pose = armor.pose;
        armor.pose = tf2_buffer_->transform(pose, target_frame_).pose;
        }
        //清除倾斜角度过大或过远的装甲板
        msg->armors.erase(std::remove_if(msg->armors.begin(), msg->armors.end(), [](const rm_msg_interfaces::msg::Armor& armor){
        return std::abs(armor.pose.position.z) > 1.2 ||  Eigen::Vector2d(armor.pose.position.x, armor.pose.position.y).norm();
        }), msg->armors.end());
 
       if(this->tracker_->tracker_state == LOST)
       {

       }
       else 
       {


       }

    }

    std::shared_ptr<Tracker> TrackerNode::initTracker()
    {
        std::shared_ptr<Tracker> tracker;
        tracker->tracker_state = LOST;
        tracker->max_distance = this->declare_parameter("/tracker/max_distance", 100.0);
        tracker_->dt_ = 0;
        //先预函数
        auto f = [this](const Eigen::VectorXd& x)
        {
            Eigen::VectorXd x_new = x;
            x_new(0) += x(1) * tracker_->dt_; 
            x_new(2) += x(3) * tracker_->dt_;
            x_new(4) += x(5) * tracker_->dt_;
            x_new(6) += x(7) * tracker_->dt_;
            return x_new;
        };
        
        return tracker;
    }


}