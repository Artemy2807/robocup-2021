#pragma once
#include <chrono>

using std::chrono::time_point;
using std::chrono::high_resolution_clock;

namespace extra {
    class Timer {
    public:
        Timer() = default;
        
        void start();
        void stop();
    
        /* 
        * Функция возвращает время между вызовами start() и stop()
        * в миллисекундах 
        */
        unsigned long millis();
        /* 
        * Функция возвращает время между вызовами start() и stop()
        * в микросекундах 
        */
        unsigned long micros();
    
    private:
        time_point<high_resolution_clock> start_t_,
                                end_t_;
                                
    };
} // namespace extra
