#include "timer.hpp"
#include <iostream>

namespace extra {

    void Timer::start() {
        start_t_ = high_resolution_clock::now();
    }

    void Timer::stop() {
        end_t_ = high_resolution_clock::now();
    }

    /* 
    * Функция возвращает время между вызовами start() и stop()
    * в миллисекундах 
    */
    long int Timer::millis() {
        return static_cast<long int>(
                std::chrono::duration_cast<std::chrono::milliseconds>(end_t_ - start_t_).count());
    }
    /* 
    * Функция возвращает время между вызовами start() и stop()
    * в микросекундах 
    */
    long Timer::micros() {
        return static_cast<long int>(
                std::chrono::duration_cast<std::chrono::microseconds>(end_t_ - start_t_).count());
    }

} // namespace extra
