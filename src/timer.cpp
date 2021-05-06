#include "timer.hpp"

namespace extra {

    void Timer::start() {
        start_t_ = std::chrono::high_resolution_clock::now();
    }

    void Timer::stop() {
        end_t_ = std::chrono::high_resolution_clock::now();
    }

    /* 
    * Функция возвращает время между вызовами start() и stop()
    * в миллисекундах 
    */
    long int Timer::millis() {
        return static_cast<long int>((end_t_ - start_t_).count() * 1000.0);
    }
    /* 
    * Функция возвращает время между вызовами start() и stop()
    * в микросекундах 
    */
    long Timer::micros() {
        return static_cast<long int>((end_t_ - start_t_).count() * 1000000.0); 
    }

} // namespace extra
