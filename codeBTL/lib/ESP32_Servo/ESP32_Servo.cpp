#include <Arduino.h>
#include "ESP32_Servo.h"

ESP32_Servo::ESP32_Servo()
{
    _maxDuty = (1 << SERVO_RES) - 1;
}

void ESP32_Servo::begin()
{
    ledcSetup(SERVO_CHANNEL, SERVO_FREQ, SERVO_RES);
    ledcAttachPin(SERVO_PIN, SERVO_CHANNEL);
    write(CLOSE_ANGLE);  // Dam bao nap dong khi khoi dong
}

void ESP32_Servo::feedFish()
{
    write(FEED_ANGLE);        // Mo nap
    vTaskDelay(pdMS_TO_TICKS(FEED_DURATION));     // Doi
    write(CLOSE_ANGLE);       // Dong nap
}

void ESP32_Servo::write(uint8_t angle)
{
    angle = constrain(angle, 0, 180);
    uint32_t us = map(angle, 0, 180, SERVO_MIN_US, SERVO_MAX_US);
    writeMicroseconds(us);
}

void ESP32_Servo::writeMicroseconds(uint16_t us)
{
    us = constrain(us, SERVO_MIN_US, SERVO_MAX_US);
    uint32_t duty = (us * _maxDuty) / 20000;  // 20ms
    ledcWrite(SERVO_CHANNEL, duty);
}

void ESP32_Servo::detach()
{
    ledcDetachPin(SERVO_PIN);
}
