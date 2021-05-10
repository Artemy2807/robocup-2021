#include "teller.hpp"
#include <bitset>

/*
 * Функция для создания потока общения с Arduino.
 * ptr - указатель на параметр типа void, для получения основной информации о модели. 
 */
void* arduino_fnc(void* ptr) {
	System& system = *((System*)ptr);

    Engine engine;
    char message[4];
    unsigned char data_l[5];
    i2c::I2C controller;
    i2c::Device device_i2c = system.device_i2c.read();
	if (!(controller.open(device_i2c)))
		return nullptr;

	Engine prev_engine;

	while (!(system.close_thr.read())) {
		engine = system.engine.read();
        if((engine.speed_ != prev_engine.speed_) || (engine.angle_ != prev_engine.angle_) || (engine.direction_ != prev_engine.direction_)){
            message[0] = 'D';
            message[1] = engine.angle_;
            message[2] = engine.speed_;
            message[3] = engine.direction_;
            controller.write((void*)(&message[0]), 4);
            
            prev_engine = engine;
        }

        if(controller.read((void*)(&data_l[0]), sizeof(data_l)) == (sizeof(data_l))) {
            if(data_l[0] == 127){
                unsigned long distance;
                distance = data_l[4] << 24 | data_l[3] << 16;
                distance |= data_l[2] << 8 | data_l[1];

                if(distance > system.distance.read()) 
                    system.distance.write(distance);
            }
        }
		usleep(10000);
	}
    message[0] = 'D';
    message[1] = 90;
    message[2] = 0;
    message[3] = 1;
    controller.write((void*)(&message[0]), 4);

	controller.close();

	return nullptr;
}
