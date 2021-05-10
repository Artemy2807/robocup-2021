#include "proc-loop.hpp"

void* loop_fnc(void* ptr) {
    System& system = *((System*)ptr);
    Engine engine;
    bool hold_tr_red = false;
    const uint8_t base_angle = 90;
    const bool base_direction = true;
    
#if (__LINE_INCLUDE__ == 1)
    const uint8_t base_speed = 75;
#else
    const uint8_t base_speed = 30;
#endif
    
    while(!(system.close_thr.read())) {
        engine = system.engine.read();
        engine.angle_ = base_angle;
        if(!(hold_tr_red)) 
            engine.speed_ = base_speed;
        engine.direction_ = base_direction;
    
#if defined(__LINE_INCLUDE__)
        static const float range_integral = 50;
        static const uint8_t range_angle = 30;
        static float integral = 0.0f,
                    kp = 0.0f,
                    ki = 0.0f,
                    kd = 0.0f;
        
#if (__LINE_INCLUDE__ == 1)
        kp = 0.2f;
        ki = 0.0f;
        kd = 0.6f;
#endif

        Object<Line>* new_line = nullptr;
        new_line = system.line.wait(new_line);

        if(new_line != nullptr/* && ((new_line->obj->center_) != -1)*/) {
            if(((new_line->obj->center_) != -1)) {
                static int32_t prev_center = new_line->obj->center_;
                float error = new_line->obj->set_point_ - new_line->obj->center_,
                    delta = new_line->obj->center_ - prev_center,
                    output = .0f;
                prev_center = new_line->obj->center_;
                
                output += error * kp;
                output += delta * kd;
                integral += error * ki;
                integral = (integral > range_integral ? range_integral : (integral < -range_integral ? -range_integral : integral));
                output += integral;
                
                engine.angle_ = (uint8_t)(base_angle + (output > range_angle ? range_angle : (output < -range_angle ? -range_angle : output)));
            }
            new_line->free();
        }
#endif
 
#if defined(__CV_INCLUDE__)
        static bool hold = false;
        static unsigned long distance_prev = 0;
        static extra::Timer timer_tr,
                    timer_reaction;
        static const float k_speed = 2.0;
        static const unsigned long base_reaction_time = 2000;
        static const unsigned long base_reaction_dist = 10;
                    
        static std::vector<Sign> signs;
        static std::vector<Holder> holders;

        if(!(hold)) {
            signs = system.signs.read();
            
            Sign sign;
            sign_t sign_code = none_s;
            if(!signs.empty()) {
                sign = *std::min_element(signs.begin(), signs.end(), 
                                        [](const Sign& a, const Sign& b){
                                            return ((int)a.sign_ < (int)b.sign_ ? true : false);
                                        });
                sign_code = sign.sign_;
            }
            
            if(sign_code == tr_red_s || sign_code == tr_yellow_s || sign_code == tr_yellowred_s)
                sign_code = tr_red_s;
            
            if(sign_code == tr_red_s && !(hold_tr_red)) {
                hold_tr_red = true;
                timer_tr.start();
            }else if((sign_code == tr_red_s || sign_code == none_s) && hold_tr_red) {
                timer_tr.stop();
                if(timer_tr.millis() > 18000) {
                    hold_tr_red = false;
                    sign_code = tr_green_s;
                }
            }
            
            if(sign_code != none_s)
                hold = true;
            
            switch(sign_code) {
#if defined(__FULL_REACTION__)
                case stop_s:
                {
                    holders.push_back(Holder(base_speed, sign.distance_, true));
                    holders.push_back(Holder(0, base_reaction_time, false));
                    break;
                }
                case rough_s:
                {
                    if(sign.distance_ > base_reaction_dist)
                        holders.push_back(Holder(base_speed, sign.distance_ - base_reaction_dist, true));
                    holders.push_back(Holder(((float)base_speed / k_speed), base_reaction_dist * 2, true));
                    break;
                }
                case over_adv_s:
                {
                    if(sign.distance_ > base_reaction_dist)
                        holders.push_back(Holder(base_speed, sign.distance_ - base_reaction_dist, true));
                    holders.push_back(Holder(((float)base_speed * k_speed), base_reaction_dist * 2, true));
                    break;
                }
                case adv_s:
                {
                    holders.push_back(Holder(base_speed, sign.distance_, true));
                    holders.push_back(Holder(0, 100, false));
                    holders.push_back(Holder(base_speed, base_reaction_dist, true, !base_direction));
                    holders.push_back(Holder(0, base_reaction_time, false));
                    break;
                }
#else
                case stop_s:
                {
                    holders.push_back(Holder(0, base_reaction_time, false));
                    break;
                }
                case rough_s:
                {
                    holders.push_back(Holder(((float)base_speed / k_speed), base_reaction_time, false));
                    break;
                }
                case over_adv_s:
                {
                    holders.push_back(Holder(((float)base_speed * k_speed), base_reaction_time, false));
                    break;
                }
                case adv_s:
                {
                    holders.push_back(Holder(0, 100, false));
                    holders.push_back(Holder(base_speed, base_reaction_dist, true, !base_direction));
                    holders.push_back(Holder(0, base_reaction_time, false));
                    break;
                }
#endif
                case tr_red_s:
                {
                    holders.push_back(Holder(0, 100, false));
                    break;
                }
                case tr_green_s:
                {
                    holders.push_back(Holder(base_speed, 10, true));
                    hold_tr_red = false;
                    break;
                }
                default:
                {
                    break;
                }
            }
        }else{
            if(!(holders.empty())) {
                Holder& holder = holders.front();
                if(holder.init_) {
                    timer_reaction.start();
                    distance_prev = system.distance.read();
                    holder.init_ = false;
                }

                timer_reaction.stop();
                if((!(holder.toggle_) && timer_reaction.millis() >= holder.thr_) || 
                    (holder.toggle_ && ((system.distance.read()) - distance_prev) >= holder.thr_)) {
                    holders.erase(holders.begin());
                }else {
                    engine.speed_ = holder.speed_;
                    engine.direction_ = holder.direction_;
                }
            }else
                hold = false;
        }
#endif
        
        system.engine.write(engine);
        usleep(25000);
    }
    return nullptr;
}
