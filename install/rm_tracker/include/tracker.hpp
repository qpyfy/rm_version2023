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


namespace rm_auto_aim
{

class Tracker

{
public:
    Tracker();

    int tracker_state;
    int tracker_number;

    double max_distance;


    geometry_msgs::msg::Point tracker_position;

};




}
#endif
