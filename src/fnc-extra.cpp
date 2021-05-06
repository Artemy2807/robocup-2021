#include "fnc-extra.hpp"

namespace extra {

    void findCandidate(const cv::Mat& input, cv::Mat& gray, 
                    std::vector<cv::Rect>& min_rects,
                    cv::Size blur_kernel, 
                    int area_min, 
                    int area_max) 
    {

        static std::vector<contour_t> contours;
        static contour_t approx;

        min_rects.clear();

        cv::Mat prop;
        cv::GaussianBlur(input, gray, blur_kernel, 2);
        cv::cvtColor(gray, gray, CV_COLOR_BGR2GRAY);
        cv::Canny(gray, prop, 50, 150, 3);
            
        cv::findContours(prop, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
        std::for_each(std::begin(contours), std::end(contours), 
                    [&](const contour_t& a){
                        float area = fabs(cv::contourArea((cv::Mat)a));
                        if(area < area_min || area > area_max)
                            return;
                            
                        cv::approxPolyDP(a, approx, cv::arcLength(cv::Mat(a), true) * 0.01, true);
                        cv::Rect rect = cv::boundingRect(approx);
                        min_rects.push_back(rect);
                    });
    }
} // namespace extra
