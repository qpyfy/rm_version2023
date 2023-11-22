#include<armor.hpp>

namespace rm_auto_aim{
Light::Light(cv::RotatedRect rect)
{
    cv::Point2f vertices[4];
    this->rect = rect;
    rect.points(vertices);
    std::sort(vertices, vertices + 4, [](const cv::Point2f & a, const cv::Point2f & b) { return a.y < b.y; });
    top_point = (vertices[0]+vertices[1])/2;
    bottom_point = (vertices[2]+vertices[3])/2;
    center_point = (top_point+bottom_point)/2;
    length = cv::norm(top_point - bottom_point);
    weight = cv::norm(vertices[0] - vertices[1]);
    angle = std::atan2(std::abs(top_point.x - bottom_point.x), std::abs(top_point.y - bottom_point.y));
    angle = angle / CV_PI*180;
}
Light::Light(){}

Armor::Armor(const Light& l1,const Light& l2,int type)
 {
    this->center_point = (l1.center_point+l2.center_point)/2;
    this->length = cv::norm(l1.center_point - l2.center_point);
    this->weight = (l1.weight+l2.weight)/2;
    this->color = l1.color;
    this->type = type;
    if (l1.center_point.x < l2.center_point.x) {
      left_light = l1, right_light = l2;
    } else {
      left_light = l2, right_light = l1;
    }
 } 
 Armor::Armor()
 {
     RCLCPP_INFO(rclcpp::get_logger("node"),"Armor");
 }
}   
// namespace rm_auto_aim