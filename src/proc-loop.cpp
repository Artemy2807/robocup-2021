#include "proc-loop.hpp"

static const uint8_t base_speed = 30;
static const unsigned long base_reaction_time = 2000;

void* loop_fnc(void* ptr) {
    System& system = *((System*)ptr);
    
    std::vector<Sign> signs;
    std::vector<Holder> holders;
    Engine engine;
    bool hold_tr_red = false,
        hold = false;
    unsigned long distance_prev = 0;
    extra::Timer timer_tr,
                timer_reaction;
    
    while(!(system.close_thr.read())) {
        engine = system.engine.read();
        engine.speed_ = base_speed;
        
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
                case stop_s:
                {
                    //holders.push_back(Holder(base_speed, sign.distance_, true));
                    holders.push_back(Holder(0, base_reaction_time, false));
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
                    distance_prev = engine.distance_;
                    holder.init_ = false;
                }
                
                timer_reaction.stop();
                if((!(holder.toggle_) && timer_reaction.millis() >= holder.thr_) || 
                    (holder.toggle_ && (engine.distance_ - distance_prev) >= holder.thr_)) {
                    holders.erase(holders.begin());
                }else {
                    engine.speed_ = holder.speed_;
                }
            }else
                hold = false;
        }
        
        std::cout << (int)engine.speed_ << std::endl;
        
        system.engine.write(engine);
        usleep(10000);
    }
    return nullptr;
}
