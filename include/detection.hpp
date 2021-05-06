#pragma once
#include <set>
#include <vector>
#include "timer.hpp"
#include "system.hpp"
#include "structs.hpp"
#include "cv-define.hpp"
#include "fnc-extra.hpp"

using rects_t = std::vector<cv::Rect>;

inline float get_distance(cv::Rect sign_area);

/*
* Функция для создания потока нахождение дорожных знаков на изображение.
* ptr - указатель на параметр типа void, для получения основной информации о модели. 
*/
void* detection_fnc(void* ptr);
