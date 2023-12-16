#ifndef TRACKER_H
#define TRACKER_H
// Eigen
#include <Eigen/Eigen>

// ROS
#include <geometry_msgs/msg/point.hpp>
#include <geometry_msgs/msg/quaternion.hpp>
#include <geometry_msgs/msg/vector3.hpp>

// STD
#include <memory>
#include <string>
#include <vector>

#include "extended_kalman_filiter.hpp"
#include "rm_msg_interfaces/msg/armors.hpp"
#include "rm_msg_interfaces/msg/armor.hpp"

namespace rm_auto_aim
{

    class Tracker

    {
    public:
        Tracker();
        enum State
        {
            LOST,
            FINDING,
            TRACKING,
            TEMP_LOST,
        } tracker_state;

        std::string tracker_number;
        rm_msg_interfaces::msg::Armor tracker_armor;
        double max_match_distance;
        double max_match_yaw;
        Eigen::VectorXd target_state;

        KalmanFilter ekf;
        // 计算预测步长
        rclcpp::Time last_time_;
        // 预测步长
        double dt_;
        int lost_time_;
        int detect_time_;
        int lost_thres_;
        int tracking_thres_;
        double last_yaw_;

        void init(rm_msg_interfaces::msg::Armors::SharedPtr msg);
        void updata(rm_msg_interfaces::msg::Armors::SharedPtr msg);
        Eigen::Vector3d getArmorPositionFromState(const Eigen::VectorXd &x);
        void initEKF(rm_msg_interfaces::msg::Armor armor);

        double orientationToYaw(const geometry_msgs::msg::Quaternion &q);
        // 装甲板跳变处理
        void handleArmorJump(const rm_msg_interfaces::msg::Armor &current_armor);

        geometry_msgs::msg::Point tracker_position;
    };

}
#endif
