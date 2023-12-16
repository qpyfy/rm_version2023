#include <tracker_node.hpp>

#include <memory>
#include <vector>


namespace rm_auto_aim
{

    ArmorTrackerNode::ArmorTrackerNode(const rclcpp::NodeOptions &options) : Node("tracker_node")
    {
        RCLCPP_INFO(this->get_logger(), "Initializing tracker node");
        this->tracker_ = initTracker();

        // 初始化
        this->timer_ = std::make_shared<tf2_ros::CreateTimerROS>(
            this->get_node_base_interface(),
            this->get_node_timers_interface());
        this->armor_sub_.subscribe(this, "armors",rmw_qos_profile_sensor_data);
        this->target_frame_ = this->declare_parameter("target_frame", "odom");
        this->tf2_buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
        this->tf2_buffer_->setCreateTimerInterface(this->timer_);
        this->tf2_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf2_buffer_);
        this->tf2_filter_ = std::make_shared<tf2_ros::MessageFilter<rm_msg_interfaces::msg::Armors>>(
            armor_sub_,
            *this->tf2_buffer_,
            target_frame_, 100,
            this->get_node_logging_interface(),
            this->get_node_clock_interface(),
            std::chrono::duration<int>(1));
        this->tf2_filter_->registerCallback(&ArmorTrackerNode::ArmorCallback, this);
        this->tracker_pub_ = rclcpp::create_publisher<rm_msg_interfaces::msg::Tracker>(this, "tracker", 10);
    }

    void ArmorTrackerNode::ArmorCallback(const rm_msg_interfaces::msg::Armors::SharedPtr &msg)
    {
        // 坐标转换
        for (auto armor : msg->armors)
        {
            geometry_msgs::msg::PoseStamped pose;
            pose.pose = armor.pose;
            armor.pose = tf2_buffer_->transform(pose, target_frame_).pose;
        }
        // 清除倾斜角度过大或过远的装甲板
        msg->armors.erase(std::remove_if(msg->armors.begin(), msg->armors.end(), [](const rm_msg_interfaces::msg::Armor &armor)
                                         { return std::abs(armor.pose.position.z) > 1.2 || Eigen::Vector2d(armor.pose.position.x, armor.pose.position.y).norm(); }),
                          msg->armors.end());

        // 预测
        rclcpp::Time now = this->get_clock()->now();
        rm_msg_interfaces::msg::Tracker tracker_msg;
        if (this->tracker_->tracker_state == Tracker::LOST)
        {
            this->tracker_->init(msg);
            tracker_msg.trackering = false;
        }
        else
        {
            this->tracker_->dt_ = (now - this->tracker_->last_time_).seconds();
            this->tracker_->lost_thres_ = static_cast<int>(lost_time_thres_ / this->tracker_->dt_);
            this->tracker_->updata(msg);

            if(this->tracker_->tracker_state == Tracker::TRACKING)
            {
                tracker_msg.trackering = true;
                tracker_msg.id = this->tracker_->tracker_number;
                tracker_msg.position.x = this->tracker_->target_state[0];
                tracker_msg.position.y = this->tracker_->target_state[2];
                tracker_msg.position.z = this->tracker_->target_state[4];
                tracker_msg.yaw = this->tracker_->target_state[6];
                tracker_msg.radius = this->tracker_->target_state[8];
                tracker_msg.velocity.x = this->tracker_->target_state[1];
                tracker_msg.velocity.y = this->tracker_->target_state[3];
                tracker_msg.velocity.z = this->tracker_->target_state[5];

            }
            else
            {
                tracker_msg.trackering = false;
            }
            this->tracker_pub_->publish(tracker_msg);



        }
    }

    std::shared_ptr<Tracker> ArmorTrackerNode::initTracker()
    {
            std::shared_ptr<Tracker> tracker;
            tracker = std::make_unique<Tracker>();
            tracker->tracker_state = Tracker::LOST;
        tracker->max_match_distance = this->declare_parameter("max_match_distance", 100.0);
        tracker->dt_ = 0.0;
        // 先预函数
        auto f = [this](const Eigen::VectorXd &x)
        {
            Eigen::VectorXd x_new = x;
            x_new(0) += x(1) * tracker_->dt_;
            x_new(2) += x(3) * tracker_->dt_;
            x_new(4) += x(5) * tracker_->dt_;
            x_new(6) += x(7) * tracker_->dt_;
            return x_new;
        };
        // 状态转移矩阵
        auto j_f = [this](const Eigen::VectorXd &x)
        {
            Eigen::MatrixXd f(9, 9);
            double dt = tracker_->dt_;
            f << 1, dt, 0, 0, 0, 0, 0, 0, 0,
                0, 1, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 1, dt, 0, 0, 0, 0, 0,
                0, 0, 0, 1, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 1, dt, 0, 0, 0,
                0, 0, 0, 0, 0, 1, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 1, dt, 0,
                0, 0, 0, 0, 0, 0, 0, 1, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 1;

            return f;
        };
        // 过程噪声矩阵
        s2qxyz_ = declare_parameter("ekf.sigma2_q_xyz", 20.0);
        s2qyaw_ = declare_parameter("ekf.sigma2_q_yaw", 100.0);
        s2qr_ = declare_parameter("ekf.sigma2_q_r", 800.0);
        auto u_q = [this]()
        {
            Eigen::MatrixXd q(9, 9);
            double t = tracker_->dt_, x = s2qxyz_, y = s2qyaw_, r = s2qr_;
            double q_x_x = pow(t, 4) / 4 * x, q_x_vx = pow(t, 3) / 2 * x, q_vx_vx = pow(t, 2) * x;
            double q_y_y = pow(t, 4) / 4 * y, q_y_vy = pow(t, 3) / 2 * x, q_vy_vy = pow(t, 2) * y;
            double q_r = pow(t, 4) / 4 * r;
            //    xc      v_xc    yc      v_yc    za      v_za    yaw     v_yaw   r
            q << q_x_x, q_x_vx, 0, 0, 0, 0, 0, 0, 0,
                q_x_vx, q_vx_vx, 0, 0, 0, 0, 0, 0, 0,
                0, 0, q_x_x, q_x_vx, 0, 0, 0, 0, 0,
                0, 0, q_x_vx, q_vx_vx, 0, 0, 0, 0, 0,
                0, 0, 0, 0, q_x_x, q_x_vx, 0, 0, 0,
                0, 0, 0, 0, q_x_vx, q_vx_vx, 0, 0, 0,
                0, 0, 0, 0, 0, 0, q_y_y, q_y_vy, 0,
                0, 0, 0, 0, 0, 0, q_y_vy, q_vy_vy, 0,
                0, 0, 0, 0, 0, 0, 0, 0, q_r;
            return q;
        };

        // 获取p_post的雅可比矩阵
        auto j_h = [](const Eigen::VectorXd &x)
        {
            Eigen::MatrixXd h(4, 9);
            double yaw = x(6), r = x(8);
            // clang-format off
    //    xc   v_xc yc   v_yc za   v_za yaw         v_yaw r
    h <<  1,   0,   0,   0,   0,   0,   r*sin(yaw), 0,   -cos(yaw),
          0,   0,   1,   0,   0,   0,   -r*cos(yaw),0,   -sin(yaw),
          0,   0,   0,   0,   1,   0,   0,          0,   0,
          0,   0,   0,   0,   0,   0,   1,          0,   0;
    return h;
  };
//观测噪声矩阵
  r_xyz_factor = declare_parameter("ekf.r_xyz_factor", 0.05);
  r_yaw = declare_parameter("ekf.r_yaw", 0.02);
  auto u_r = [this](const Eigen::VectorXd & z) {
    Eigen::DiagonalMatrix<double, 4> r;
    double x = r_xyz_factor;
    r.diagonal() << abs(x * z[0]), abs(x * z[1]), abs(x * z[2]), r_yaw;
    return r;
  };
  //观测函数
  auto h = [](const Eigen::VectorXd & x) {
    Eigen::VectorXd z(4);
    double xc = x(0), yc = x(2), yaw = x(6), r = x(8);
    z(0) = xc - r * cos(yaw);  // xa
    z(1) = yc - r * sin(yaw);  // ya
    z(2) = x(4);               // za
    z(3) = x(6);               // yaw
    return z;
  };
    Eigen::DiagonalMatrix<double,9> q;
    q.setIdentity();
    //初始化卡尔曼滤波器
    tracker->ekf = KalmanFilter(f,h,u_q,u_r,j_f,j_h,q);
    RCLCPP_INFO(this->get_logger(), "KalmanFilter init");   

        return tracker;
    }


}
#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(rm_auto_aim::ArmorTrackerNode)


