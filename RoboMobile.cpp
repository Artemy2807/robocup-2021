#include <unistd.h>
#include <pthread.h>

#include "mode.hpp"
#include "system.hpp"
#include "proc-loop.hpp"
#include "capture.hpp"
#include "detection.hpp"
#include "teller.hpp"
#include "navigation.hpp"

int main() {
#if (defined(__CV_INCLUDE__) || defined(__LINE_INCLUDE__))
    System system;

	pthread_t capture_thr;
	pthread_create(&capture_thr, nullptr, capture_fnc, &system);

    pthread_t arduino_thr;
	pthread_create(&arduino_thr, NULL, arduino_fnc, &system);
    
    pthread_t loop_thr;
	pthread_create(&loop_thr, NULL, loop_fnc, &system);

#if defined(__LINE_INCLUDE__)
    pthread_t navigation_thr;
	pthread_create(&navigation_thr, NULL, navigation_fnc, &system);
#endif

#if defined(__CV_INCLUDE__)
    pthread_t detection_thr;
	pthread_create(&detection_thr, NULL, detection_fnc, &system);
#endif
    
    pthread_join(loop_thr, nullptr);
    pthread_join(arduino_thr, nullptr);

#if defined(__LINE_INCLUDE__)
    pthread_join(navigation_thr, nullptr);
#endif
#if defined(__CV_INCLUDE__)
    pthread_join(detection_thr, nullptr);
#endif
    
    system.close_capture.write(true);
    pthread_join(capture_thr, nullptr);
#endif    

    return 0;
}
 
