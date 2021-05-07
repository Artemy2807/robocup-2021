#pragma once
#include <algorithm>

#include "cv-define.hpp"
#include "structs.hpp"
#include "system.hpp"
#include "timer.hpp"

struct Holder {
    uint8_t speed_;
    unsigned long thr_;
    bool toggle_,
        init_,
        direction_;
    
    Holder(uint8_t speed = 0, unsigned long thr = 0, bool toggle = false, bool direction = true):
        speed_(speed),
        thr_(thr),
        toggle_(toggle),
        init_(true),
        direction_(direction)
    {
    }
};

void* loop_fnc(void* ptr);
