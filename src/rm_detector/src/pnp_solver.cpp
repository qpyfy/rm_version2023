#include <pnp_solver.hpp>

#include <geometry_msgs/msg/point.hpp>
#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
// STD
#include <array>
#include <vector>
#include <detector.hpp>

namespace rm_auto_aim
{
    PnpSolver::PnpSolver()
    {
        
    }
    PnpSolver::PnpSolver(const cv::Mat & camera_matrix, const cv::Mat & dist_coeffs)
    {
        camera_matrix_ = camera_matrix;
        dist_coeffs_ = dist_coeffs;
    }

    bool PnpSolver::solvePnP(const Armor & armor, cv::Mat & rvec, cv::Mat & tvec)
    {
        std::vector<cv::Point2f> armor_points;
        armor_points.push_back(armor.left_light.center_point);
        armor_points.push_back(armor.right_light.center_point);
        armor_points.push_back(armor.right_light.top_point);
        armor_points.push_back(armor.right_light.bottom_point);

        if (armor.type == SMALL_ARMOR)
        {
         return  cv::solvePnP(small_armor_points_, armor_points, camera_matrix_, dist_coeffs_, rvec, tvec,false,cv::SOLVEPNP_EPNP);
        }
        else
        {
         return  cv::solvePnP(large_armor_points_, armor_points, camera_matrix_, dist_coeffs_, rvec, tvec,false,cv::SOLVEPNP_EPNP);
        }

    }

    float PnpSolver::calculateDistanceToCenter(const cv::Point2f & image_point)
{
  float cx = camera_matrix_.at<double>(0, 2);
  float cy = camera_matrix_.at<double>(1, 2);
  return cv::norm(image_point - cv::Point2f(cx, cy));
}
} // namespace