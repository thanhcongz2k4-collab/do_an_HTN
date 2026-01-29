#ifndef ESP32_SERVO_H
#define ESP32_SERVO_H

#include <Arduino.h>

class ESP32_Servo {
public:
    ESP32_Servo();

    void attach(uint8_t pin, uint8_t channel);
    void write(uint8_t angle);   // 0–180
    void detach();

private:
    uint8_t  _pin;
    uint8_t  _channel;
    uint16_t _minUs;
    uint16_t _maxUs;
    uint32_t _freq;
    uint8_t  _resolution;

    void writeMicroseconds(uint16_t us);
};

#endif
