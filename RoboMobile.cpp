#include <unistd.h>
#include <pthread.h>

#include "system.hpp"
#include "capture.hpp"
#include "detection.hpp"

int main() {
    System system;

    // Поток чтения видео с Вебкамеры
	pthread_t capture_thr;
	pthread_create(&capture_thr, nullptr, capture_fnc, &system);
    
    pthread_t detection_thr;
	pthread_create(&detection_thr, NULL, detection_fnc, &system);
    
	pthread_join(capture_thr, nullptr);
    pthread_join(detection_thr, nullptr);
    return 0;
}
 
