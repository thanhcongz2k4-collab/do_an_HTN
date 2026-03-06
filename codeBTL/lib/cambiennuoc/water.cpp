#include "water.h"

WaterSensor::WaterSensor() {
    minValue = 0;       // Giá trị mặc định khi không có nước
    maxValue = 4095;     // Giá trị mặc định khi ngập hoàn toàn
}

void WaterSensor::begin() {
    pinMode(WATER_SENSOR_PIN, INPUT);
}

void WaterSensor::setCalibration(int minVal, int maxVal) {
    minValue = minVal;
    maxValue = maxVal;
}

int WaterSensor::readRaw() {
    // Đọc trung bình 10 lần để giảm nhiễu ADC
    long sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += analogRead(WATER_SENSOR_PIN);
        delayMicroseconds(100);
    }
    return (int)(sum / 10);
}

int WaterSensor::readPercent() {
    int value = readRaw();
    value = constrain(value, minValue, maxValue);
    return map(value, minValue, maxValue, 0, 100);
}

bool WaterSensor::isLow() {
    return readPercent() < WATER_LEVEL_LOW;  // Dưới 50%
}

bool WaterSensor::isMedium() {
    int percent = readPercent();
    return (percent >= WATER_LEVEL_LOW && percent < WATER_LEVEL_MEDIUM);  // 50% - 80%
}

bool WaterSensor::isFull() {
    return readPercent() >= WATER_LEVEL_MEDIUM;  // Trên 80%
}

String WaterSensor::getStatusString() {
    int percent = readPercent();
    if (percent < WATER_LEVEL_LOW) return "THAP";
    if (percent < WATER_LEVEL_MEDIUM) return "TRUNG BINH";
    return "DAY";
}
