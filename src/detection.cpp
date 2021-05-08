#include "detection.hpp"

/*
 * Функция возвращает расстояние от чёрной линии до вебкамеры модели.
 * line_width - параметр типа int32_t, ширина чёрной линии в пикселях.
 */
float get_distance(cv::Rect sign_area){
    return (1520.f / (float)sign_area.width);
}

const cv::Size hog_descriptor_size(64, 64);
const int size_models = 4;
const std::vector<std::string> m_paths = { "../models/stop.xml", 
                                            "../models/rough.xml", 
                                            "../models/adv.xml", 
                                            "../models/over-adv.xml" };
const std::vector<sign_t> sign_codes = { stop_s, rough_s, adv_s, over_adv_s };

/*
 * Функция для создания потока распознавания дорожных знаков на изображении.
 * ptr - указатель на параметр типа void, для получения основной информации о модели. 
 */
void* detection_fnc(void* ptr) {
	System &system = *((System*)ptr);
    
    cv::Mat gray, roi, prop;
    Object<cv::Mat>* obj_cur = nullptr;
    cv::Rect sign_area = system.sign_area.read();

    signs_t signs_global, signs_detect;
    rects_t rects_cand;
    std::vector<extra::contour_t> approx_c;
    extra::Timer timer;
    
    cv::HOGDescriptor hog;
    hog.winSize = hog_descriptor_size;
    std::vector<CvPtrSVM> models;
    
    omp_set_num_threads(size_models);
    for(unsigned int i = 0; i < m_paths.size(); i++) {
#if (CV_MAJOR_VERSION >= 3)
        models.push_back(CvSVM::load(m_paths[i]));
#else
        models.push_back(CvSVM());
        models.back().load(m_paths[i]);
#endif
    }
    
    timer.start();
    while(!system.close_thr.read()) {
        // Получаем изображение с вебкамеры
		obj_cur = system.frame.wait(obj_cur);
        if(obj_cur == nullptr)
            continue;

		cv::Mat& frame = (*(obj_cur->obj));

        extra::findCandidate(frame(sign_area), gray, prop, rects_cand, approx_c);
        
        for(unsigned int i = 0; i < rects_cand.size(); i++) {
            float dl = (float)rects_cand[i].width / (float)rects_cand[i].height;
            if(fabs(1.0 - dl) < 0.3) {
                std::vector<float> descriptors;
            
                cv::resize(gray(rects_cand[i]), roi, hog.winSize);
                hog.compute(roi, descriptors, cv::Size(8, 8));
                roi.create(1, descriptors.size(), CV_32FC1);
                cv::transpose(cv::Mat(descriptors), roi);
                
                #pragma omp parallel for
                for(unsigned long j = 0; j < models.size(); j++) {
                    if(models[j]->predict(roi) > .4f) {
                        signs_detect.push_back(Sign(sign_codes[j], 
                                            rects_cand[i], 
                                            get_distance(rects_cand[i])));
                        rectangle(frame(sign_area), rects_cand[i], cv::Scalar(255, 0, 0), 2);
                        //break;
                    }
                }
            }
            
#if (__LINE_INCLUDE__)
            Object<Line>* new_line = nullptr;
            new_line = system.line.wait(new_line);
            if(new_line == nullptr)
                continue;
            
            if((new_line->obj->road_type_) != stopline_r) {
                new_line->free();
                continue;
            }
            new_line->free();
#endif
            if(fabs(0.5 - dl) < 0.3) {
                if(!cv::isContourConvex(approx_c[i]))
                    continue;
                
                unsigned long black_pxl = 0;
                
                {
                    for (int32_t rows = 0; rows < rects_cand[i].height; rows++) {
                        uint8_t* row = (uint8_t*)gray(rects_cand[i]).ptr<uint8_t>(rows);
                        for (int32_t cols = 0; cols < rects_cand[i].width; cols++)
                            if (row[cols] < 60) black_pxl++;
                    }
                }

                if(black_pxl < (rects_cand[i].height * rects_cand[i].width) * .5f)
                    continue;
                const unsigned int column = rects_cand[i].width * 0.5;
                unsigned int while_pxl = 0,
                            white_pxl_offset = 0;
                    
                cv::Mat rr = gray(rects_cand[i]);
                for (int32_t offset = 0; offset < rects_cand[i].height; offset++) 
                    if (rr.at<uint8_t>(offset, column) > 70) {
						while_pxl++;
						white_pxl_offset += offset;
					}
                if (while_pxl <= 7) continue;
                float light_pos = ((float)white_pxl_offset / (float)while_pxl) / (float)rr.rows;
                Sign traffic_light;
                traffic_light.area_ = rects_cand[i];
				traffic_light.sign_ = tr_off_s;

                if (light_pos > 0.20 && light_pos < 0.55) {
                    traffic_light.sign_ = tr_red_s;
                    rectangle(frame(sign_area), rects_cand[i], cv::Scalar(0, 0, 255), 2);
                }else if (light_pos > 0.55 && light_pos < 0.85){
                    traffic_light.sign_ = tr_green_s;
                    rectangle(frame(sign_area), rects_cand[i], cv::Scalar(0, 255, 0), 2);
                }
                signs_detect.push_back(traffic_light);
            }
        }
        
        timer.stop();
        long int spend_time = timer.millis();
        
		for (unsigned int i = 0; i < signs_detect.size(); i++) {
			for (unsigned int j = i + 1; j < signs_detect.size(); j++) {
				if (signs_detect[j].sign_ == signs_detect[i].sign_) {
					signs_detect.erase(signs_detect.begin() + j);
					j--;
				}
			}
		}

		for (unsigned int i = 0; i < signs_global.size(); i++) {
			for (unsigned int j = 0; j < signs_detect.size(); j++) {
				if (signs_global[i].sign_ == signs_detect[j].sign_) {
					signs_global[i] = signs_detect[j];
					signs_detect.erase(signs_detect.begin() + j);
				}
			}

			signs_global[i].time_ += spend_time;
			if (signs_global[i].time_ > 200)
				signs_global.erase(signs_global.begin() + i);
		}

		for (unsigned int i = 0; i < signs_detect.size(); i++)
			signs_global.push_back(signs_detect[i]);
        
        for(unsigned int i = 0; i < signs_global.size(); i++) {
            std::cout << "sign code: " << (int)signs_global[i].sign_ << std::endl;
            std::cout << "sign time: " << (int)signs_global[i].time_ << std::endl;
            cv::rectangle(frame, signs_global[i].area_, cv::Scalar(0, 255, 0), 2);
        }

		system.signs.write(signs_global);
		signs_detect.clear();
        //cv::imshow("frame", frame);
        //cv::imshow("gray", gray);
        //cv::imshow("prop", prop);
        //cv::waitKey(0);
        obj_cur->free();
        timer.start();
    }
    return nullptr;
}
