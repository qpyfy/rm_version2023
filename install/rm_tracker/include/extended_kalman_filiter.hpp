#ifndef ARMOR_PROCESSOR__KALMAN_FILTER_HPP_
#define ARMOR_PROCESSOR__KALMAN_FILTER_HPP_

#include <Eigen/Dense>
#include <functional>
#include <rclcpp/rclcpp.hpp>
#include <angles/angles.h>
#include <tf2/LinearMath/Matrix3x3.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/convert.h>
#include <rm_msg_interfaces/msg/armor.hpp>

namespace rm_auto_aim
{
class KalmanFilter
{
    public:
    KalmanFilter();

    KalmanFilter(
    std::function<Eigen::VectorXd(Eigen::VectorXd)> f,
    std::function<Eigen::VectorXd(Eigen::VectorXd)> h,
    std::function<Eigen::MatrixXd()> u_q,
    std::function<Eigen::MatrixXd(Eigen::VectorXd)> u_r,
    std::function<Eigen::MatrixXd(Eigen::VectorXd)> j_f,
    std::function<Eigen::MatrixXd(Eigen::VectorXd)> j_h, 
    Eigen::MatrixXd P);

    void initState(Eigen::VectorXd armor);
    void initEKF(rm_msg_interfaces::msg::Armor armor);


    Eigen::MatrixXd predict();
    Eigen::MatrixXd update(Eigen::VectorXd z);

    std::function<Eigen::VectorXd(Eigen::VectorXd)> f;
    std::function<Eigen::VectorXd(Eigen::VectorXd)> h;
    //过程噪声矩阵
    std::function<Eigen::MatrixXd()> updata_q;
    Eigen::MatrixXd Q;
    //观测噪声矩阵
    std::function<Eigen::MatrixXd(Eigen::VectorXd)> updata_r;
    Eigen::MatrixXd R;
    //状态转移矩阵的雅可比矩阵
    std::function<Eigen::MatrixXd(Eigen::VectorXd)> jacobian_f;
    Eigen::MatrixXd F;
    //观测矩阵的雅可比矩阵
    std::function<Eigen::MatrixXd(Eigen::VectorXd)> jacobian_h;
    Eigen::MatrixXd H;
    int n;
    Eigen::MatrixXd P_pre;
    Eigen::MatrixXd P_post;
    Eigen::VectorXd X_pre;
    Eigen::VectorXd X_post;
    Eigen::MatrixXd K;
    Eigen::MatrixXd I;



};

}
#endif