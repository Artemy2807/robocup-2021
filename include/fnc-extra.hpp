#pragma once
#include "cv-define.hpp"
#include <vector>

namespace extra {
    using contour_t = std::vector<cv::Point>;

    void findCandidate(const cv::Mat& input, cv::Mat& gray, cv::Mat& prop,
                    std::vector<cv::Rect>& min_rects, 
                    cv::Size blur_kernel = cv::Size(3, 3), 
                    int area_min = 700, 
                    int area_max = 16000);
} // namespace extra
