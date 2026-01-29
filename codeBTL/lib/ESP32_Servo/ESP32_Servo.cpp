#include <Arduino.h>
#include "ESP32_Servo.h"

ESP32_Servo::ESP32_Servo()
{
    _pin        = 255;
    _channel    = 0;
    _minUs      = 500;
    _maxUs      = 2500;
    _freq       = 50;     // 50Hz
    _resolution = 16;     // PWM 16-bit
}

void ESP32_Servo::attach(uint8_t pin, uint8_t channel)
{
    _pin = pin;
    _channel = channel;

    ledcSetup(_channel, _freq, _resolution);
    ledcAttachPin(_pin, _channel);
}

void ESP32_Servo::write(uint8_t angle)
{
    angle = constrain(angle, 0, 180);
    uint32_t us = map(angle, 0, 180, _minUs, _maxUs);
    writeMicroseconds(us);
}

void ESP32_Servo::writeMicroseconds(uint16_t us)
{
    us = constrain(us, _minUs, _maxUs);

    uint32_t maxDuty = (1 << _resolution) - 1;
    uint32_t duty = (us * maxDuty) / 20000; // 20ms

    ledcWrite(_channel, duty);
}

void ESP32_Servo::detach()
{
    if (_pin != 255) {
        ledcDetachPin(_pin);
        _pin = 255;
    }
}
