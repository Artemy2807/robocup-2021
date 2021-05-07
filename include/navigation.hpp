#pragma once         
#include <unistd.h>

#include "structs.hpp"
#include "system.hpp" 
#include "timer.hpp"

/*
 * Функция, которая проверяет является ли пиксель черным.
 */
inline bool black_pxl(uint8_t* row, uint16_t x);

/*
 * Функция для нахождения чёрной линии на изображение.
 */
void proc_line(const cv::Mat& frame, int32_t scan, Line& line);
/*
 * Функция для создания потока нахождение линии на изображение.
 * ptr - указатель на параметр типа void, для получения основной информации о модели. 
 */
void* navigation_fnc(void* ptr);
