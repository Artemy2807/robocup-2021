#include "capture.hpp"

/*
 * Функция для создания потока чтения кадров с вебкамеры.
 * ptr - указатель на параметр типа void, для получения основной информации о модели. 
 */
void* capture_fnc(void* ptr) {
    System &system = *((System *)ptr);
    // Таймер для расчёта ФПС
    extra::Timer timer_fps;
    cv::Mat frame;
    // Переменная, которая хранит значение ФПС
    uint8_t fps_count;

    int cap_id = system.device_id.read();
	cv::VideoCapture cap(cap_id, CV_CAP_V4L);
	if (!cap.isOpened()) {
        LOG(WARNING) << "Cant't open video source " << cap_id << "\n";
        system.close_thr.write(true);
		return nullptr;
	}
	
    // Получаем размер кадра
    cv::Point frame_size = system.frame_size.read();  
	cap.set(CV_CAP_PROP_FRAME_WIDTH, frame_size.x);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, frame_size.y);
	cap.set(CV_CAP_PROP_FPS, 30);

    timer_fps.start();
    while(!(system.close_thr.read())) {
        if(cap.grab()) {
            Object<cv::Mat>* obj_new = new Object<cv::Mat>();
            cap.read(frame);
            obj_new.write(frame);
            system.frame.push(obj_new);
        }

        // Проверяем таймер, если прошла одна секунда, то выводим
        // количество ФПС и обнуляем таймер, в противном случае прибавляем единицу
        // к сётчику ФПС
        timer_fps.stop();
        if(timer_fps.millis() >= 1000) {
            LOG(NOTICE) << "FPS count " << fps_count << std::endl;
            fps_count = 0;
            timer_fps.start();
        }else fps_count++;
    }
    return nullptr;
}
