#pragma once
#include <pthread.h>
#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include <vector>

#include "queue.hpp"
#include "i2c-api.hpp"
#include "structs.hpp"
#include "logger.hpp"
#include "cv-define.hpp"

struct System {
	Queue<cv::Mat> frame;
	Object<Line> line;
	Object<cv::Point> frame_size;
	Object<Engine> engine;
	Object<std::vector<Sign>> signs;
	Object<uint32_t> server_port;
	Object<bool> close_thr;
	Object<cv::Rect> sign_area;

	System(cv::Size frame_size = cv::Size(640, 480), cv::Rect sign_area = cv::Rect(0, 0, 640, 480), uint32_t server_port = 1111);
};
