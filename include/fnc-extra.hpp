#pragma once
#include "cv-define.hpp"
#include <vector>

namespace extra {
    using contour_t = std::vector<cv::Point>;

    void findCandidate(const cv::Mat& input, cv::Mat& gray, 
                    std::vector<cv::Rect>& min_rects, 
                    cv::Size blur_kernel = cv::Size(5, 5), 
                    int area_min = 700, 
                    int area_max = 16000);
} // namespace extra
