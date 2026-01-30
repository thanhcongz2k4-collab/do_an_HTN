#ifndef WATER_H
#define WATER_H

#include <Arduino.h>

class WaterSensor {
private:
    uint8_t analogPin;

public:
    WaterSensor(uint8_t pin);
    void begin();
    int readRaw();          // Giá trị ADC thô
    int readPercent();      // % mực nước (0–100%)
};

#endif
