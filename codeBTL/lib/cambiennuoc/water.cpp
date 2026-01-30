#include "water.h"

WaterSensor::WaterSensor(uint8_t pin) {
    analogPin = pin;
}

void WaterSensor::begin() {
    pinMode(analogPin, INPUT);
}

int WaterSensor::readRaw() {
    return analogRead(analogPin); // ESP32: 0 - 4095
}

int WaterSensor::readPercent() {
    int value = readRaw();
    value = constrain(value, 0, 4095);
    return map(value, 0, 4095, 0, 100);
}
