// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp/duration.hpp>
#include <rclcpp/qos.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <rclcpp/node.hpp>


// STD
#include <algorithm>
#include <cmath>
#include <vector>


#include <detector.hpp>
#include <armor.hpp>

namespace rm_auto_aim
{


  Detector::Detector()
  {
    
  }
   cv::Mat Detector::PreProcess(const cv::Mat & img)
   {
       cv::Mat bin_img;
       cv::cvtColor(img,bin_img,cv::COLOR_BGR2GRAY);
       cv::threshold(bin_img,bin_img,binary_thres,255,cv::THRESH_BINARY);
       return bin_img;
   }
 

std::vector<Light> Detector::findLights(const cv::Mat& bin_img)
{
    std::vector<Light> lights;
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(bin_img,contours,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_NONE);
    for(auto contour:contours)
    {
      if(contour.size()<5)continue;
        auto rect = cv::minAreaRect(contour);
        Light light(rect);
        if(isLight(light))
        {
        //获取灯条的颜色
      auto rect = light.boundingRect();
      int sum_r = 0, sum_b = 0;
      if (  
        0 <= rect.x && 0 <= rect.width && rect.x + rect.width <= img.cols && 0 <= rect.y &&
        0 <= rect.height && rect.y + rect.height <= img.rows) {
        auto roi = img(rect);
        for (int i = 0; i < roi.rows; i++) {
          for (int j = 0; j < roi.cols; j++) {
            if (cv::pointPolygonTest(contour, cv::Point2f(j + rect.x, i + rect.y), false) >= 0) 
            {
              sum_r += roi.at<cv::Vec3b>(i, j)[0];
              sum_b += roi.at<cv::Vec3b>(i, j)[2];
            }
          }
        }
        
        }
        light.color = sum_r > sum_b ? Red : Blue;
        lights.push_back(light);
        }
   }
   if(lights.size()<1)RCLCPP_INFO(rclcpp::get_logger("node"),"lights.size()<1");
   else RCLCPP_INFO(rclcpp::get_logger("node"),"lights.size()=%ld",lights.size());
   return lights;
}


bool Detector::isLight(const Light& light)
{
    if(light.angle>light_max_angle)return false;
    if((float)light.weight/light.length < light_min_l_w_ratio)return false;
    if((float)light.weight/light.length > light_max_l_w_ratio)return false;
    return true;
}

std::vector<Armor> Detector::matchArmor(const std::vector<Light>& lights)
{
    std::vector<Armor> armors;
    for(auto l1 = lights.begin();l1 != lights.end();l1++)
    {
        for(auto l2 = l1+1;l2 != lights.end();l2++)
        {
            if(containLight(*l1,*l2,lights))continue;
            int type =  isArmor(*l1,*l2);
            if(type)
            {
                Armor armor(*l1,*l2,type);             
                armors.push_back(armor);               
            }
        }
        
    } 

           return armors;
}


int Detector::isArmor(const Light& l1,const Light& l2)
{
  if(l1.color != light_detector_color&&l1.color != l2.color)return NO_ARMOR;
  
  cv::Point2f diff = l1.center_point - l2.center_point;
  float angle = std::abs(std::atan(diff.y / diff.x)) / CV_PI * 180;
  if (angle > armor_max_angle) return NO_ARMOR;

  float avg_light_length = (l1.length + l2.length) / 2;
  float center_distance = cv::norm(l1.center_point - l2.center_point) / avg_light_length;
  int type;
  if(center_distance < large_armor_max_center_disatance && center_distance > large_armor_min_center_disatance)
  {
    type = LARGE_ARMOR;
  }
  else if(center_distance > small_armor_min_center_disatance && center_distance < small_armor_max_center_disatance)
  {
    type = SMALL_ARMOR;
  }
  else
  {
    type = NO_ARMOR;
    
  }
  return type;
}

bool Detector::containLight(
  const Light & light_1, const Light & light_2, const std::vector<Light> & lights)
{
  auto points = std::vector<cv::Point2f>{light_1.top_point, light_1.bottom_point, light_2.top_point, light_2.bottom_point};
  auto bounding_rect = cv::boundingRect(points);

  for (const auto & test_light : lights) {
    if (test_light.center == light_1.center || test_light.center == light_2.center) continue;

    if (
      bounding_rect.contains(test_light.top_point) || 
      bounding_rect.contains(test_light.bottom_point) ||
      bounding_rect.contains(test_light.center)) 
      {
      return true;
    }
  }

  return false;
}
}
 // namespace rm_auto_aim