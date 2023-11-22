#ifndef NUMBER_CLASSIFIER_H
#define NUMBER_CLASSIFIER_H
// OpenCV
#include <opencv2/opencv.hpp>

// STL
#include <cstddef>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "armor.hpp"

#define Red 0
#define Blue 1 


#define NO_ARMOR 0
#define SMALL_ARMOR 1
#define LARGE_ARMOR 2
namespace rm_auto_aim
{
    class NumberClassifier
    {
        public:
        std::string model_path;
        std::string config_path;
        std::vector<std::string> class_names_;
        std::vector<std::string> ignore_classes_;
        cv::dnn::Net net_;
        double threshold_;

        void extractNumbers(const cv::Mat src, std::vector<Armor> & armors);

        void classifyNumbers(std::vector<Armor> & armors);
    };
}
#endif