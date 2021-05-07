#include "navigation.hpp"

/*
 * Функция, которая проверяет является ли пиксель черным.
 */
bool black_pxl(uint8_t* row, uint16_t x) {
    uint8_t r = row[3 * x + 2],
            g = row[3 * x + 1],
            b = row[3 * x];
            
    return (b <= 100 && abs(b - g) < 25 && abs(b - r) < 25 && abs(r - g) < 25);
}

static int32_t center_first = -1,
            center_seria = -1,
            max_diff_ = 0,
            min_diff_ = 0,
            max_row = -1;
            
static bool seen_prev = false,
            first_stop_line = true;

/*
 * Функция для нахождения чёрной линии на изображение.
 */
void proc_line(const cv::Mat& frame, int32_t scan, Line& line) {
    int32_t line_diff = -1;    
    uint8_t* row = (uint8_t*)frame.ptr<uint8_t>(scan);

	if (line.old_center_ == -1) {
        int32_t closest_center = -1,
                closest_diff = -1;
		line.center_ = -1;
        
		for (int32_t i = 0; i < frame.cols; i++) {
			if (black_pxl(row, i)) {
				int32_t start_point = i,
                        black_pxln = 3;
				while (black_pxln > 1 && i + 3 < frame.cols) {
					i += 3;
					black_pxln = 0;
					for (int32_t j = 0; j < 3; ++j)
						black_pxln += int(black_pxl(row, (i + j)));
				}
				
				if ((i - start_point) < 10)
                    continue;
				else {
					line.center_ = (i + start_point) / 2;
					line_diff = (i - start_point);
                    if (abs(320 - line.center_) < abs(320 - closest_center)) {
						closest_center = line.center_;
						closest_diff = line_diff;
					}
				}
			}
		}
		line.center_ = closest_center;
		line_diff = closest_diff;
	}else {
		if (black_pxl(row, line.old_center_)) {
			int32_t i, black_pxln;
			for (i = line.old_center_ + 1; i + 3 < frame.cols; i += 3) {
				black_pxln = 0;
				for (int32_t j = 0; j < 3; ++j)
					black_pxln += (unsigned int)(black_pxl(row, (i + j)));
                
				if (black_pxln <= 1)
					break;
			}
			line_diff = i;

			for (i = line.old_center_ - 1; i >= 3; i -= 3) {
				black_pxln = 0;
				for (int j = 0; j < 3; ++j)
					black_pxln += (unsigned int)(black_pxl(row, (i - j)));
                
				if (black_pxln <= 1)
					break;
			}

			line.center_ = (i + line_diff) / 2;
			line_diff = line_diff - i;
			if (abs(line_diff - i) < 25) {
				line.center_ = -1;
				line.old_center_ = -1;
				return;
			}
		}else{
			line.center_ = -1;
			line.old_center_ = -1;
			return;
		}
	}
	
	if(center_first == -1 && line.center_ != -1)
        center_first = line.center_;
    
    if(line.center_ != -1 && abs(line.center_ - center_first) > 150) {
        line.center_ = -1;
        line.old_center_ = -1;
    }

	if(line.center_ != -1) {
		if(max_diff_ < line_diff || max_diff_ == 0) {
			max_diff_ = line_diff;
            max_row = scan;
		}
		
        if(min_diff_ > line_diff || min_diff_ == 0) {
            min_diff_ = line_diff;
            center_seria = line.center_;
		}
	}

	line.old_center_ = line.center_;
}
/*
 * Функция для создания потока нахождение линии на изображение.
 * ptr - указатель на параметр типа void, для получения основной информации о модели. 
 */
void* navigation_fnc(void* ptr) {
    System &system = *((System*)ptr);

    Line line;
    extra::Timer seen_prev_t;
	Object<cv::Mat>* obj_cur = nullptr;
    
    const unsigned int scan_row = 470;
    
    seen_prev_t.start();
	while (!(system.close_thr.read())) {
		obj_cur = system.frame.wait(obj_cur);

        if(obj_cur == nullptr)
            continue;
		cv::Mat& frame = (*(obj_cur->obj));
        
        Object<Line>* new_line = new Object<Line>();

        center_first = -1;
		max_diff_ = 0;
        min_diff_ = 0;

		for(unsigned int k = 0; k <= 80; k+=5)
        	proc_line(frame, (scan_row - k), line);
        
        line.center_ = center_first;
        line.old_center_ = center_first;

        line.road_type_ = unknown_r;
        if(max_diff_ != 0 && min_diff_ != 0) {
            if(max_diff_ > 300 && max_diff_ > (min_diff_ * 6)) {
                if(max_row > 445) {
                    line.old_center_ = -1;
                    proc_line(frame, max_row - 50, line);
                    line.old_center_ = line.center_;
                    
                    line.road_type_ = crossroad_r;
                    seen_prev = false;
                }
            }else{
                seen_prev_t.stop();
                if(max_diff_ > 100 && max_diff_ > (min_diff_ * 2.5) && (first_stop_line || seen_prev || seen_prev_t.millis() >= 5000)) {
                    //line.center_ = center_seria;
                    //line.old_center_ = center_seria;
                    if(max_row > 450) {
                        line.old_center_ = -1;
                        proc_line(frame, max_row - 50, line);
                        
                    }
                    line.road_type_ = stopline_r;
                    first_stop_line = false;
                    seen_prev = true;
                }else seen_prev = false;
            }
        }
        
        if(line.road_type_ == crossroad_r || seen_prev)
            seen_prev_t.start();

        *(new_line->obj) = line;
		system.line.push(new_line);
        obj_cur->free();
	}
	return nullptr;
}
