#include "water.h"

// Khoi tao cac moc hieu chuan mac dinh cho ADC muc nuoc.
WaterSensor::WaterSensor() {
    minValue = 0;       // Giá trị mặc định khi không có nước
    midValue = 2048;    // Giá trị mặc định ở mức 50%
    maxValue = 4095;    // Giá trị mặc định khi ngập hoàn toàn
}

// Cau hinh chan doc cam bien nuoc o che do input.
void WaterSensor::begin() {
    pinMode(WATER_SENSOR_PIN, INPUT);
}

// Cap nhat bo 3 diem hieu chuan: can duoi, trung gian va can tren.
void WaterSensor::setCalibration(int minVal, int midVal, int maxVal) {
    minValue = minVal;
    midValue = midVal;
    maxValue = maxVal;
}

// Doc gia tri ADC thuc te, lay trung binh nhieu mau de giam nhieu.
int WaterSensor::readRaw() {
    // Đọc trung bình 10 lần để giảm nhiễu ADC
    long sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += analogRead(WATER_SENSOR_PIN);
        delayMicroseconds(100);
    }
    return (int)(sum / 10);
}

// Chuyen gia tri ADC sang phan tram muc nuoc dua tren duong cong 2 doan.
int WaterSensor::readPercent() {
    int value = readRaw();
    value = constrain(value, minValue, maxValue);
    if (value <= midValue) {
        return map(value, minValue, midValue, 0, 50);
    } else {
        return map(value, midValue, maxValue, 50, 100);
    }
}

// Tra ve true neu muc nuoc duoi nguong thap da dinh nghia.
bool WaterSensor::isLow() {
    return readPercent() < WATER_LEVEL_LOW;  // Dưới 50%
}

// Tra ve true neu muc nuoc nam trong khoang trung binh.
bool WaterSensor::isMedium() {
    int percent = readPercent();
    return (percent >= WATER_LEVEL_LOW && percent < WATER_LEVEL_MEDIUM);  // 50% - 80%
}

// Tra ve true neu muc nuoc dat hoac vuot nguong day.
bool WaterSensor::isFull() {
    return readPercent() >= WATER_LEVEL_MEDIUM;  // Trên 80%
}

// Tra ve chuoi trang thai de hien thi giao dien/ghi log.
String WaterSensor::getStatusString() {
    int percent = readPercent();
    if (percent < WATER_LEVEL_LOW) return "THAP";
    if (percent < WATER_LEVEL_MEDIUM) return "TRUNG BINH";
    return "DAY";
}
