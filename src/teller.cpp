#include "teller.hpp"

/*
 * Функция для создания потока общения с Arduino.
 * ptr - указатель на параметр типа void, для получения основной информации о модели. 
 */
void* arduino_fnc(void* ptr) {
	System& system = *((System*)ptr);

    Engine engine;
    char message[3];
    
    i2c::I2C controller;
    i2c::Device device_i2c = system.device_i2c.read();
	if (!(controller.open(device_i2c)))
		return nullptr;
    
    union {
        unsigned long data_;
        char mode_;
        
        char data_l_[(sizeof(data_) + sizeof(mode_))];
    } ld;

	while (!(system.close_thr.read())) {
		engine = system.engine.read();
        
        message[0] = engine.angle_;
        message[1] = engine.speed_;
        message[2] = engine.direction_;
        if(controller.write((void*)(&message[0]), 3) < 0)
            break;
        
        if(controller.read((void*)(&ld.data_l_[0]), sizeof(ld.data_l_))) {

        }

        system.engine.write(engine);
		usleep(10000);
	}
	controller.close();
	return nullptr;
}
