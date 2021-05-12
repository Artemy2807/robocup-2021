#include <Wire.h>
#include <Servo.h>

#define MOTOR_PWM         5
#define MOTOR_A1          7
#define MOTOR_B1          8
#define SERVO_PIN         9

class Motor {
private:
  int pwm, 
    a1, 
    b1;

  bool direction_;
public:
  Motor(int pwm_, int a1_, int b1_):
    pwm(pwm_),
    a1(a1_),
    b1(b1_)
  {
    pinMode(pwm, OUTPUT);
    pinMode(a1_, OUTPUT);
    pinMode(b1_, OUTPUT);

    speed(0);
    stop(false);
    direction_ = true;
  }

  /*
   * flag = false, BREAKEGND
   * flag = true, BREAKEVCC
   */
  void stop(bool flag) {
    if(flag) {
      digitalWrite(a1, HIGH);
      digitalWrite(b1, HIGH);
      speed(255);
    }else{
      digitalWrite(a1, LOW);
      digitalWrite(b1, LOW);
      speed(0);
    }
  }

  /*
   * flag = false, назад
   * flag - true, вперёд
   */
  void direction(bool flag) {
    direction_ = flag;
    if(flag) {
      digitalWrite(a1, LOW);
      digitalWrite(b1, HIGH);
    }else{
      digitalWrite(a1, HIGH);
      digitalWrite(b1, LOW);
    }
  }

  void speed(int speed_) {
    analogWrite(pwm, speed_);
  }

  bool get_direction() { return direction_; }
};

struct Regulator {
  int output = 0, output_old = 0,
    set_point = 0;
  float input = 0.0, integral = 0.0;
  float kp = 0.5, 
        ki = 0.2;
};

void receive_data(int bytes);
void request_data();
void encoder_update();

Servo servo;
Motor motor(MOTOR_PWM, MOTOR_A1, MOTOR_B1);

uint8_t turn = 90, old_turn = 90;
bool direction = true;
Regulator reg;
unsigned long time = 0,
              time_current = 0;
volatile unsigned long encoder_pulse = 0,
                      encoder_pulse_total = 0;
                      
const uint8_t min_pwm_motor = 35;
                      

void setup() {
  Serial.begin(9600);
  Wire.begin(8);
  // Регистрирует функцию, вызываемую, когда ведомое устройство получает передачу от ведущего
  Wire.onReceive(receive_data);
  // Регистрирует функцию, вызываемую, когда ведущее устройство получает передачу от ведомого
  Wire.onRequest(request_data);

  attachInterrupt(0, encoder_update, FALLING);
  attachInterrupt(1, encoder_update, FALLING);

  motor.direction(true);

  servo.attach(SERVO_PIN);
  servo.write(turn);

  time = millis();
}

void loop() {
  if(turn != old_turn) {
    servo.write(turn);
    old_turn = turn;
  }
  
  time_current = millis();

  if(reg.set_point == 0) {
      reg.integral = 0;
      reg.output = 0;
      motor.speed(0);
  }
  
  if(time_current - time > 300) {
    time = time_current;
    
    reg.input = (float)encoder_pulse / (float)(0.3f * 0.924f);
    encoder_pulse = 0;
    if(reg.set_point != 0 && !(reg.output >= min_pwm_motor && reg.input == 0)) {
      reg.output_old = reg.output;
      float error = (float)reg.set_point - reg.input;
      
      reg.integral += error * reg.ki;
      if(reg.integral > 200) 
        reg.integral = 200;
      
      reg.output = (int)(error * reg.kp + (float)reg.integral);
      reg.output = (reg.output > 255 ? 255 : (reg.output < 0 ? 0 : reg.output));
      reg.output = (reg.output + reg.output_old) * (float)0.5;
      
      motor.direction(!direction);
      motor.speed(reg.output);
    }
    /*
    Serial.print(reg.set_point);
    Serial.print(' ');
    Serial.print(reg.input);
    Serial.print(' ');
    Serial.print(reg.output);
    Serial.println(' ');
    */
  }
}

void receive_data(int bytes) {
  while(Wire.available()) {
    if((char)(Wire.read()) == 'D') {
      turn = (uint8_t)(Wire.read());
      reg.set_point = (uint8_t)(Wire.read());
      direction = (bool)(Wire.read());
    }
  }
  
  /*
  Serial.print("set point: ");
  Serial.print(reg.set_point);
  Serial.print("    turn: ");
  Serial.print(turn);
  Serial.print("    direction: ");
  Serial.println(direction);
  */
}

void request_data() {
  union {
    unsigned long data;
    byte data_l[sizeof(data)];
  } ul;

  ul.data = (unsigned long)((float)encoder_pulse_total / 0.924f);
  //Serial.println(ul.data );

  Wire.write(127);
  Wire.write(ul.data_l, sizeof(ul.data));
}

void encoder_update() {
  encoder_pulse++;
  encoder_pulse_total++;
}
