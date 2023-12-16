#include <tracker.hpp>

#include <angles/angles.h>
#include <tf2/LinearMath/Matrix3x3.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/convert.h>

#include <rclcpp/logger.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

// STD
#include <cfloat>
#include <memory>
#include <string>

namespace rm_auto_aim
{


    Tracker::Tracker(){}
    
    
    void Tracker::init(rm_msg_interfaces::msg::Armors::SharedPtr msg)
    {
        if (msg->armors.empty())
        {
            return;
        }
        this->tracker_armor = msg->armors[0];
        this->tracker_state = Tracker::FINDING;
        double min_distance_to_image_center = msg->armors[0].distance_to_image_center;
        for (auto armor : msg->armors)
        {
            if (armor.distance_to_image_center < min_distance_to_image_center)
            {
                this->tracker_armor = armor;
                min_distance_to_image_center = armor.distance_to_image_center;
            }
        }

        this->tracker_number = this->tracker_armor.number;
        this->ekf.initEKF(this->tracker_armor);
    }

    void Tracker::updata(rm_msg_interfaces::msg::Armors::SharedPtr msg)
    {
        if (msg->armors.empty())
        {
            this->tracker_state = Tracker::LOST;
            return;
        }
        bool matched = false;
        // 获取先预装甲板的位置
        Eigen::VectorXd ekf_predict = this->ekf.predict();
        this->target_state = this->getArmorPositionFromState(ekf_predict);

        double min_position_diff = DBL_MAX;
        double yaw_diff = DBL_MAX;
        int same_number_count = 0;
        // 遍历所有装甲板 获取离预测位姿最近的装甲板
        for (auto armor : msg->armors)
        {
            if (armor.number == this->tracker_number)
            {

                same_number_count++;
                Eigen::Vector3d armor_position(armor.pose.position.x, armor.pose.position.y, armor.pose.position.z);
                double position_diff = (armor_position - target_state).norm();
                if (position_diff < min_position_diff)
                {
                    min_position_diff = position_diff;
                    yaw_diff = abs(orientationToYaw(armor.pose.orientation) - ekf_predict(6));
                    this->tracker_armor = armor;
                }
            }
        }

        if (min_position_diff < this->max_match_distance && yaw_diff < this->max_match_yaw)
        {
            this->tracker_state = Tracker::TRACKING;
            matched = true;
            auto position = this->tracker_armor.pose.position;
            double yaw = orientationToYaw(this->tracker_armor.pose.orientation);
            // 获取观测装甲板的位置
            Eigen::VectorXd z = Eigen::Vector4d(position.x, position.y, position.z, yaw);
            // 更新滤波器 并获取后验装甲板的位置
            target_state = this->ekf.update(z);
        }
        else if (same_number_count == 1 && yaw_diff > this->max_match_yaw)
        {
            // 如果只有一个装甲板 但是偏航角差距过大 则认为跳变
            handleArmorJump(this->tracker_armor);
        }
        else
        {
            RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Tracker lost");
        }
        // 限制装甲板的yaw轴变化速率
        if (target_state(8) < 0.12)
        {
            target_state(8) = 0.12;
            ekf.initState(target_state);
        }
        else if (target_state(8) > 0.4)
        {
            target_state(8) = 0.4;
            ekf.initState(target_state);
        }


        //更新状态
        if (tracker_state == Tracker::FINDING)
        {
            if (matched)
            {
                detect_time_++;
                if (detect_time_ > tracking_thres_)
                {
                    detect_time_ = 0;
                    tracker_state = Tracker::TRACKING;
                }
            }
            else
            {
                detect_time_ = 0;
                tracker_state =  Tracker::LOST;
            }
        }
        else if (tracker_state == Tracker::TRACKING)
        {
            if (!matched)
            {
                tracker_state = Tracker::TEMP_LOST;
                lost_time_++;
            }
        }
        else if (tracker_state == Tracker::TEMP_LOST)
        {
            if (!matched)
            {
                lost_time_++;
                if (lost_time_ > lost_thres_)
                {
                    lost_time_ = 0;
                    tracker_state = Tracker::LOST;
                }
            }
            else
            {
                tracker_state = Tracker::TRACKING;
                lost_time_ = 0;
            }
        }
    }
    // 位姿转坐标
    Eigen::Vector3d Tracker::getArmorPositionFromState(const Eigen::VectorXd &x)
    {
        double xc = x(0), yc = x(2), za = x(4);
        double yaw = x(6), r = x(8);
        double xa = xc - r * cos(yaw);
        double ya = yc - r * sin(yaw);
        return Eigen::Vector3d(xa, ya, za);
    }

    void Tracker::initEKF(rm_msg_interfaces::msg::Armor armor)
    {
        Eigen::VectorXd target_state(9);
        double xa = armor.pose.position.x;
        double ya = armor.pose.position.y;
        double za = armor.pose.position.z;
        double yaw = orientationToYaw(armor.pose.orientation);

        // Set initial position at 0.2m behind the target
        target_state = Eigen::VectorXd::Zero(9);
        double r = 0.26; //?
        double xc = xa + r * cos(yaw);
        double yc = ya + r * sin(yaw);
        target_state << xc, 0, yc, 0, za, 0, yaw, 0, r;
        this->ekf.initState(target_state);
    }

    double Tracker::orientationToYaw(const geometry_msgs::msg::Quaternion &q)
    {
        // 获取yaw
        tf2::Quaternion tf_q;
        tf2::fromMsg(q, tf_q);
        double roll, pitch, yaw;
        tf2::Matrix3x3(tf_q).getRPY(roll, pitch, yaw);
        // Make yaw change continuous (-pi~pi to -inf~inf)
        // 使偏航角连续变化（-pi~pi到-inf~inf）
        yaw = last_yaw_ + angles::shortest_angular_distance(last_yaw_, yaw);
        last_yaw_ = yaw;
        return yaw;
    }
    void Tracker::handleArmorJump(const rm_msg_interfaces::msg::Armor &current_armor)
    {
        double yaw = orientationToYaw(current_armor.pose.orientation);
        target_state(6) = yaw;
        RCLCPP_WARN(rclcpp::get_logger("armor_tracker"), "Armor jump!");

        auto p = current_armor.pose.position;
        Eigen::Vector3d current_p(p.x, p.y, p.z);
        Eigen::Vector3d infer_p = getArmorPositionFromState(target_state);
        if ((current_p - infer_p).norm() > max_match_distance)
        {
            double r = target_state(8);
            target_state(0) = p.x + r * cos(yaw); // xc
            target_state(1) = 0;                  // vxc
            target_state(2) = p.y + r * sin(yaw); // yc
            target_state(3) = 0;                  // vyc
            target_state(4) = p.z;                // za
            target_state(5) = 0;                  // vza
            RCLCPP_ERROR(rclcpp::get_logger("armor_tracker"), "Reset State!");
        }

        ekf.initState(target_state);
    }
}
