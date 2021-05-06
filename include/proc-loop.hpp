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
        init_;
    
    Holder(uint8_t speed = 0, unsigned long thr = 0, bool toggle = false):
        speed_(speed),
        thr_(thr),
        toggle_(toggle),
        init_(true)
    {
    }
};

void* loop_fnc(void* ptr);
