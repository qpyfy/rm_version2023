#ifndef ARMOR_DETECTOR__PNP_SOLVER_HPP_
#define ARMOR_DETECTOR__PNP_SOLVER_HPP_

#include <geometry_msgs/msg/point.hpp>
#include <opencv2/core.hpp>

// STD
#include <array>
#include <vector>

#include "armor.hpp"

namespace rm_auto_aim
{
    class PnpSolver
    {
    public:
    PnpSolver();
    PnpSolver(const cv::Mat & camera_matrix, const cv::Mat & dist_coeffs);

    bool solvePnP(const Armor & armor, cv::Mat & rvec, cv::Mat & tvec);
    float calculateDistanceToCenter(const cv::Point2f & image_point);
    cv::Mat camera_matrix_;
    cv::Mat dist_coeffs_; 
    

    float SMALL_ARMOR_WIDTH;
    float SMALL_ARMOR_HEIGHT;
    float LARGE_ARMOR_WIDTH;
    float LARGE_ARMOR_HEIGHT;

    std::vector<cv::Point3f> small_armor_points_;
    std::vector<cv::Point3f> large_armor_points_;



    };
} // namespace 



#endif