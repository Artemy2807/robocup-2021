#pragma once
#include <opencv2/opencv.hpp>

#if (CV_MAJOR_VERSION >= 3)
    #define CvSVM                       cv::ml::SVM
    #define CvPtrSVM                    cv::Ptr<CvSVM>
    #define CV_COLOR_BGR2GRAY           cv::COLOR_BGR2GRAY
    #define CV_COLOR_BGR2HSV            cv::COLOR_BGR2HSV
    #define CV_MORPH_RECT               cv::MORPH_RECT
    #define CV_MORPH_CLOSE              cv::MORPH_CLOSE
    #define CV_MORPH_OPEN               cv::MORPH_OPEN
    #define CV_RETR_TREE                cv::RETR_TREE
    #define CV_CHAIN_APPROX_SIMPLE      cv::CHAIN_APPROX_SIMPLE
    #define CV_CAP_PROP_FRAME_WIDTH     cv::CAP_PROP_FRAME_WIDTH
    #define CV_CAP_PROP_FRAME_HEIGHT    cv::CAP_PROP_FRAME_HEIGHT
    #define CV_CAP_PROP_FPS             cv::CAP_PROP_FPS
    #define CV_CAP_V4L                  cv::CAP_V4L
    #define CV_CAP_V4L2                 cv::CAP_V4L2
    #define CV_IMWRITE_JPEG_QUALITY     cv::IMWRITE_JPEG_QUALITY
#else
    #include <memory>

    #define CvPtrSVM    std::unique_ptr<CvSVM>
#endif
