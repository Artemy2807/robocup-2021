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
        /*if(*/controller.write((void*)(&message[0]), 4);// < 0)
            //break;
	prev_engine = engine;
        }
        char code;
	//if((controller.read((void*)(&code), sizeof(char) == sizeof(char))) && (code == 'C')) {
	//ul aul;
	if(controller.read((void*)(&data_l[0]), sizeof(data_l)) == (sizeof(data_l))) {
		if(data_l[0] == 127){
			//aul.data_l[0] = data_l[1];
			//aul.data_l[1] = data_l[2];
			//aul.data_l[2] = data_l[3];
			//aul.data_l[3] = data_l[4];
			unsigned long distance;
			distance = data_l[4] << 24 | data_l[3] << 16;
			distance |= data_l[2] << 8 | data_l[1];
			std::cout << std::bitset<8>(data_l[4]) << " " << std::bitset<8>(data_l[3]) << " " << std::bitset<8>(data_l[2]) << " " << std::bitset<8>(data_l[1]) << " " << std::endl;	
			//std::memcpy(&distance, &(data_l[1]), sizeof(unsigned long));
			std::cout << "teller: " << distance << " " <<  system.distance.read() << std::endl;
			if(distance > system.distance.read()) system.distance.write(distance);
		}
        }
	//}

        //system.engine.write(engine);
		usleep(10000);
	}
	//controller.close();
        message[0] = 'D';
        message[1] = 90;
        message[2] = 0;
        message[3] = 1;
        controller.write((void*)(&message[0]), 4);

	controller.close();

	return nullptr;
}
