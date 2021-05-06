#pragma once
#include <csignal>

#include "timer.hpp"
#include "system.hpp"
#include "cv-define.hpp"

extern bool program_end;

/*
 * Функция для создания потока чтения кадров с вебкамеры.
 * ptr - указатель на параметр типа void, для получения основной информации о модели. 
 */
void* capture_fnc(void* ptr);
