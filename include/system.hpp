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
	Queue<Line> line;
	Atomic<cv::Point> frame_size;
	Atomic<Engine> engine;
	Atomic<std::vector<Sign>> signs;
	Atomic<uint32_t> server_port;
	Atomic<bool> close_thr;
	Atomic<cv::Rect> sign_area;
    Atomic<int> device_id;

	System(cv::Size frame_size = cv::Size(640, 480), cv::Rect sign_area = cv::Rect(0, 0, 640, 480), uint32_t server_port = 1111);
};
