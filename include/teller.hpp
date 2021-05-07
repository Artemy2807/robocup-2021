#pragma once
#include "structs.hpp"
#include "system.hpp"
#include "i2c-api.hpp"
#include "logger.hpp"

struct SendingData {
    unsigned long data_;
    bool toggle_;
    
    SendingData(unsigned long data = 0, bool toggle = false):
            data_(data),
            toggle_(toggle)
    {
    }
};

/*
 * Функция для создания потока общения с Arduino.
 * ptr - указатель на параметр типа void, для получения основной информации о модели. 
 */
void* arduino_fnc(void* ptr);
