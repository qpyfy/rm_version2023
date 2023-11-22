
#ifndef ARMOR_HPP
#define ARMOR_HPP


#include<rclcpp/rclcpp.hpp>
#include <opencv2/core.hpp>
// STL
#include <algorithm>
#include <string>

namespace rm_auto_aim
{
    class Light:public cv::RotatedRect
    {
    public:
     cv::Point2f center_point;
        double length;
        double weight;
        float angle;
        int color;
        cv::Point2f top_point;
        cv::Point2f bottom_point;
        cv::RotatedRect rect;
      Light();
      Light(cv::RotatedRect rect);

   
    };

    class Armor 
    {
    public:
        Light left_light;
        Light right_light;
        cv::Point2f center_point;
        double length;
        double weight;
        std::string num;
        double confidence;
        int color;
        cv::Mat armor_img,number_img;
        int type;
         cv::Mat rvec,tvec;
        Armor();
        Armor(const Light & l1,const Light & l2,int type);
    };
    

    
}

#endif