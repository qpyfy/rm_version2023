#include <extended_kalman_filiter.hpp>

namespace rm_auto_aim
{
    KalmanFilter::KalmanFilter() {}

    KalmanFilter::KalmanFilter(std::function<Eigen::VectorXd(Eigen::VectorXd)> f,
                               std::function<Eigen::VectorXd(Eigen::VectorXd)> h,
                               std::function<Eigen::MatrixXd()> u_q,
                               std::function<Eigen::MatrixXd(Eigen::VectorXd)> u_r,
                               std::function<Eigen::MatrixXd(Eigen::VectorXd)> j_f,
                               std::function<Eigen::MatrixXd(Eigen::VectorXd)> j_h,
                               Eigen::MatrixXd P):
        f(f),
        h(h),
        updata_q(u_q),
        updata_r(u_r),
        jacobian_f(j_f),
        jacobian_h(j_h),
        n(P.rows()),
        P_post(P),
        X_pre(n),
        X_post(n),
        I(Eigen::MatrixXd::Identity(n, n))
       
       
    {
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "KalmanFilter init");

    };


    void KalmanFilter::initState(Eigen::VectorXd armor)
    {
        this->X_post << armor;
    }




    Eigen::MatrixXd KalmanFilter::predict()
    {
        this->Q = this->updata_q();
        this->F = this->jacobian_f(this->X_post);
        this->X_pre = this->f(this->X_post);
        this->P_pre = this->F * this->P_post * this->F.transpose() + this->Q;
        return this->X_pre;
    }

    Eigen::MatrixXd KalmanFilter::update(Eigen::VectorXd z)
    {
        this->R = this->updata_r(this->X_pre);
        this->H = this->jacobian_h(this->X_pre);
        this->K = this->P_pre * this->H.transpose() * (this->H * this->P_pre * this->H.transpose() + this->R).inverse();
        this->X_post = this->X_pre + this->K * (z - this->h(this->X_pre));
        this->P_post = (this->I - this->K * this->H) * this->P_pre;
        return this->X_post;
    }

}