#ifndef ESP32_SERVO_H
#define ESP32_SERVO_H

#include "define.h"

class ESP32_Servo {
public:
    ESP32_Servo();

    // Khoi tao servo (goi trong setup)
    void begin();

    // Cho ca an: mo nap -> doi -> dong nap
    void feedFish();

    // Dieu khien goc truc tiep (0-180)
    void write(uint8_t angle);

    // Ngat ket noi servo
    void detach();

private:
    uint32_t _maxDuty;
    void writeMicroseconds(uint16_t us);
};

#endif
