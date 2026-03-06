#include "DS18B20.h"

DS18B20Sensor::DS18B20Sensor()
    : oneWire(DS18B20_PIN), sensors(&oneWire) {
    lastTemp = 0.0;
}

void DS18B20Sensor::begin() {
    sensors.begin();
    sensors.setResolution(12);  // Độ phân giải 12-bit (chính xác nhất)
}

float DS18B20Sensor::readTempC() {
    sensors.requestTemperatures();
    float temp = sensors.getTempCByIndex(0);
    
    // Kiểm tra giá trị hợp lệ
    if (temp != DEVICE_DISCONNECTED_C) {
        lastTemp = temp;
    }
    return lastTemp;
}

float DS18B20Sensor::readTempF() {
    float tempC = readTempC();
    return tempC * 9.0 / 5.0 + 32.0;
}

float DS18B20Sensor::getLastTemp() {
    return lastTemp;
}

bool DS18B20Sensor::isConnected() {
    sensors.requestTemperatures();
    float temp = sensors.getTempCByIndex(0);
    return (temp != DEVICE_DISCONNECTED_C);
}

String DS18B20Sensor::getStatusString() {
    float temp = lastTemp;
    if (temp < 20.0) return "LANH";
    if (temp < 30.0) return "BINH THUONG";
    if (temp < 40.0) return "AM";
    return "NONG";
}
