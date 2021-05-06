#pragma once
#include <chrono>

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
        long int millis();
        /* 
        * Функция возвращает время между вызовами start() и stop()
        * в микросекундах 
        */
        long int micros();
    
    private:
        std::chrono::time_point start_t_,
                                end_t_;
                                
    };
} // namespace extra
