#pragma once
#include "cv-define.hpp"

// Структура, определяющая тип дороги перед моделью
typedef enum {
    unknown_r = 0,
    stopline_r,
    fork_r,
    crossroad_r,
} road_t;

// Перечисление, в котором хранятся возможные распознаные дорожные знаки
typedef enum {
    none_s = 0,
    stop_s,  
    rough_s,
    adv_s,
    over_adv_s,
    start_tr_green_s,
    start_tr_red_s,
    start_tr_off_s,
    tr_green_s,
    tr_yellow_s,
    tr_red_s,               
    tr_yellowred_s,
    tr_off_s,
} sign_t;

// Структура, в которую записывается информация о распознанном дорожном знаке
struct Sign {
    sign_t sign_ = none_s;
    cv::Rect area_;
    unsigned long distance_ = 0,
                time_ = 0;
    
    Sign(sign_t sign = none_s, cv::Rect area = cv::Rect(), unsigned long distance = 0, unsigned long time = 0):
        sign_(sign),
        area_(area),
        distance_(distance),
        time_(time)
    {
    }
};

// Структура, в которую записывается информация о распознанной чёрной линии.
// Чёрная линия - это направляющая линия, по которой должна двигаться модель.
struct Line {
    int32_t center_ = -1,
            old_center_ = -1,
            set_point_ = 320;
    road_t road_type_ = unknown_r;
};

// Структура, которая реализует хранение информации о движении модели
struct Engine {
	uint8_t speed_,
	        angle_;
	bool direction_;
	
	Engine(uint8_t speed = 0, uint8_t angle = 90, bool direction = true):
        speed_(speed),
        angle_(angle),
        direction_(direction)
    {
    }
};
