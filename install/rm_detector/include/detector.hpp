
#ifndef DETECTOR_HPP
#define DETECTOR_HPP
// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

// STD
#include <algorithm>
#include <cmath>
#include <vector>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp/parameter.hpp>

#include"armor.hpp"

namespace rm_auto_aim
{

  
#define Red 0
#define Blue 1 


#define NO_ARMOR 0
#define SMALL_ARMOR 1
#define LARGE_ARMOR 2
 class Detector
 {
 private:
   

 public:
    cv::Mat img;
   int binary_thres;

   //灯条参数
   int light_max_angle;
   float light_min_l_w_ratio;
   float light_max_l_w_ratio;
   int light_detector_color;

   //装甲板参数
   int armor_max_angle;
   float small_armor_max_center_disatance;
   float small_armor_min_center_disatance;
   float large_armor_min_center_disatance;
   float large_armor_max_center_disatance;



    Detector(/* args */);

    cv::Mat PreProcess(const cv::Mat & img);

    std::vector<Light> findLights(const cv::Mat& bin_img);

    bool isLight(const Light& light);

    int isArmor(const Light& l1,const Light& l2);

    bool containLight(const Light& light_1,const Light& light_2,const std::vector<Light>& lights);

    std::vector<Armor> matchArmor(const std::vector<Light>& lights);

 };
 




}

#endif
