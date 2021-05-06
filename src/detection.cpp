#include "detection.hpp"

/*
 * Функция возвращает расстояние от чёрной линии до вебкамеры модели.
 * line_width - параметр типа int32_t, ширина чёрной линии в пикселях.
 */
float get_distance(cv::Rect sign_area){
    return (1520.f / (float)sign_area.width);
}

const cv::Size hog_descriptor_size(64, 64);
const std::vector<std::string> m_paths = { "../models/stop.xml", 
                                            "../models/main-road.xml", 
                                            "../models/move-top.xml", 
                                            "../models/move-left.xml", 
                                            "../models/move-right.xml" };
const std::vector<sign_t> sign_codes = { stop_s, mainroad_s, top_s, left_s, right_s };

/*
 * Функция для создания потока распознавания дорожных знаков на изображении.
 * ptr - указатель на параметр типа void, для получения основной информации о модели. 
 */
void* detection_fnc(void* ptr) {
	System &system = *((System*)ptr);
    
    cv::Mat frame, gray, roi;
    Object<cv::Mat>* obj_cur = nullptr;
    cv::Rect sign_area = system.sign_area.read();

    auto cmp = [](Sign a, Sign b) { return (a.sign == b.sign) };
    std::set<Sign, decltype(cmp)> signs_detect(cmp),
                                signs_global(cmp);
    rects_t rects_cand;
    extra::Timer timer;
    
    cv::HOGDescriptor hog;
    hog.winSize = hog_descriptor_size;
    std::vector<CvPtrSVM> models;
    
    for(unsigned int i = 0; i < paths.size(); i++) {
#if (CV_MAJOR_VERSION >= 3)
        models.push_back(CvSVM::load(m_paths[i]));
#else
        models.push_back(CvSVM());
        models.back().load(m_paths[i]);
#endif
    }

    while(!system.close_thr.read()) {
        // Получаем изображение с вебкамеры
		obj_cur = system.frame.wait(obj_cur);
		frame = (obj_cur->read())(sign_area);

        timer.start();
        extra::findCandidate(frame, gray, rects_cand);
        
        for(unsigned int i = 0; i < rects_cand.size(); i++) {
            float dl = (float)rects_cand[i].width / (float)rects_cand[i].height;
            if(fabs(1.0 - dl) < 0.3) {
                std::vector<float> descriptors;
            
                cv::resize(gray(rects_cand[i]), roi, hog.winSize);
                hog.compute(roi, descriptors, cv::Size(8, 8));
                roi.create(1, descriptors.size(), CV_32FC1);
                cv::transpose(cv::Mat(descriptors), roi);
                
                for(unsigned long j = 0; j < models.size(); j++) {
                    if(models[i]->predict(roi) > .4f) {
                        signs_detect.insert(Sign(sign_codes[j], 
                                            rects_cand[i], 
                                            get_distance(rects_cand[i])));
                        break;
                    }
                }
            }
        }
        
        timer.stop();
        long int spend_time = timer.millis();

		for (unsigned int i = 0; i < signs_global.size(); i++) {
			for (unsigned int j = 0; j < signs_detect.size(); j++) {
				if (signs_global[i].sign_ == signs_detect[j].sign_) {
					signs_global[i] = signs_detect[j];
					signs_detect.erase(Signs.begin() + j);
				}
			}

			signs_global[i].time_ += spend_time;
			if (signs_global[i].time_ > 300)
				signs_global.erase(signs_global.begin() + i);
		}

		for (unsigned int i = 0; i < signs_detect.size(); i++)
			signs_global.push_back(signs_detect[i]);

		system.signs.write(signs_global);
		signs_detect.clear();
		obj_cur->free();
    }
    return nullptr;
}
