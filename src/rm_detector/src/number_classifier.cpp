
#ifndef NUMBER_CLASSIFIER_HPP_
#define NUMBER_CLASSIFIER_HPP_
// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

// STL
#include <algorithm>
#include <cstddef>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "armor.hpp"
#include "number_classifier.hpp"

namespace rm_auto_aim
{
    void NumberClassifier::extractNumbers(const cv::Mat  src, std::vector<Armor> & armors)
{
  // Light length in image
  const int light_length = 12;
  // Image size after warp
  const int warp_height = 28;
  const int small_armor_width = 32;
  const int large_armor_width = 54;
  // Number ROI size
  const cv::Size roi_size(20, 28);

  for (auto & armor : armors) {
    // Warp perspective transform
    cv::Point2f lights_vertices[4] = {
      armor.left_light.bottom_point, armor.left_light.top_point, armor.right_light.top_point,
      armor.right_light.bottom_point};

    const int top_light_y = (warp_height - light_length) / 2 - 1;
    const int bottom_light_y = top_light_y + light_length;
    const int warp_width = armor.type == 1 ? small_armor_width : large_armor_width;
    cv::Point2f target_vertices[4] = {
      cv::Point(0, bottom_light_y),
      cv::Point(0, top_light_y),
      cv::Point(warp_width - 1, top_light_y),
      cv::Point(warp_width - 1, bottom_light_y),
    };
    cv::Mat number_image;
    auto rotation_matrix = cv::getPerspectiveTransform(lights_vertices, target_vertices);
    cv::warpPerspective(src, number_image, rotation_matrix, cv::Size(warp_width, warp_height));

    // Get ROI
    number_image =
      number_image(cv::Rect(cv::Point((warp_width - roi_size.width) / 2, 0), roi_size));

    // Binarize
    cv::cvtColor(number_image, number_image, cv::COLOR_RGB2GRAY);
    cv::threshold(number_image, number_image, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    armor.number_img = number_image;
  }
}

void NumberClassifier::classifyNumbers(std::vector<Armor> & armors)
{


  // Classify
  for (auto & armor : armors) {
    cv::Mat input_blob; 
    cv::dnn::blobFromImage(
      armor.number_img/255,input_blob);
    this->net_.setInput(input_blob);
    cv::Mat prob = net_.forward();

     float max_prob = *std::max_element(prob.begin<float>(), prob.end<float>());
    cv::Mat softmax_prob;
    cv::exp(prob - max_prob, softmax_prob);
    float sum = static_cast<float>(cv::sum(softmax_prob)[0]);
    softmax_prob /= sum;
    cv::Mat prob_mat = softmax_prob.reshape(1, 1);

    cv::Point max_loc;
    double max_val;
    cv::minMaxLoc(prob_mat, nullptr, &max_val, nullptr, &max_loc);
    armor.num = this->class_names_[max_loc.x]; 
    armor.confidence = max_val;
  }

     armors.erase(
    std::remove_if(
      armors.begin(), armors.end(),
      [this](const Armor & armor) {
        if (armor.confidence < this->threshold_) {
          return true;
        }

        for (const auto & ignore_class : ignore_classes_) {
          if (armor.num == ignore_class) {
            return true;
          }
        }

        bool mismatch_armor_type = false;
        return mismatch_armor_type;
      }),
    armors.end());

 }
}  // namespace rm_auto_aim
#endif