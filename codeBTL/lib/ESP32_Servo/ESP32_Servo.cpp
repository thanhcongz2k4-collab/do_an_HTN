#include <Arduino.h>
#include "ESP32_Servo.h"

// Khoi tao bien duty toi da tu do phan giai PWM dang su dung.
ESP32_Servo::ESP32_Servo()
{
    _maxDuty = (1 << SERVO_RES) - 1;
}

// Cau hinh PWM cho servo va dua nap ve goc dong mac dinh khi khoi dong.
void ESP32_Servo::begin()
{
    ledcSetup(SERVO_CHANNEL, SERVO_FREQ, SERVO_RES);
    ledcAttachPin(SERVO_PIN, SERVO_CHANNEL);
    write(CLOSE_ANGLE);  // Dam bao nap dong khi khoi dong
}

// Chuoi thao tac cho ca an: mo nap, cho trong mot khoang thoi gian, sau do dong nap.
void ESP32_Servo::feedFish()
{
    write(FEED_ANGLE);        // Mo nap
    vTaskDelay(pdMS_TO_TICKS(FEED_DURATION));     // Doi
    write(CLOSE_ANGLE);       // Dong nap
}

// Dieu khien servo bang goc (0-180), ham se map sang do rong xung microsecond.
void ESP32_Servo::write(uint8_t angle)
{
    angle = constrain(angle, 0, 180);
    uint32_t us = map(angle, 0, 180, SERVO_MIN_US, SERVO_MAX_US);
    writeMicroseconds(us);
}

// Ghi truc tiep do rong xung servo (us) sau khi gioi han an toan, roi doi sang duty PWM.
void ESP32_Servo::writeMicroseconds(uint16_t us)
{
    us = constrain(us, SERVO_MIN_US, SERVO_MAX_US);
    uint32_t duty = (us * _maxDuty) / 20000;  // 20ms
    ledcWrite(SERVO_CHANNEL, duty);
}

// Tach chan khoi kenh PWM khi khong can dieu khien servo nua.
void ESP32_Servo::detach()
{
    ledcDetachPin(SERVO_PIN);
}
