#include "DS18B20.h"

// Khoi tao bus OneWire va doi tuong DallasTemperature, dat nhiet do cache ban dau.
DS18B20Sensor::DS18B20Sensor()
    : oneWire(DS18B20_PIN), sensors(&oneWire) {
    lastTemp = 0.0;
}

// Bat thu vien DS18B20 va dat do phan giai 12-bit cho do chinh xac cao.
void DS18B20Sensor::begin() {
    sensors.begin();
    sensors.setResolution(12);  // Độ phân giải 12-bit (chính xác nhất)
}

// Yeu cau cam bien do nhiet do C, neu ket qua hop le thi cap nhat gia tri cache.
float DS18B20Sensor::readTempC() {
    sensors.requestTemperatures();
    float temp = sensors.getTempCByIndex(0);
    
    // Kiểm tra giá trị hợp lệ
    if (temp != DEVICE_DISCONNECTED_C) {
        lastTemp = temp;
    }
    return lastTemp;
}

// Quy doi nhiet do C sang F tu ket qua doc moi nhat.
float DS18B20Sensor::readTempF() {
    float tempC = readTempC();
    return tempC * 9.0 / 5.0 + 32.0;
}

// Lay gia tri nhiet do hop le cuoi cung da duoc luu.
float DS18B20Sensor::getLastTemp() {
    return lastTemp;
}

// Kiem tra ket noi cam bien dua tren gia tri dac biet DEVICE_DISCONNECTED_C.
bool DS18B20Sensor::isConnected() {
    sensors.requestTemperatures();
    float temp = sensors.getTempCByIndex(0);
    return (temp != DEVICE_DISCONNECTED_C);
}

// Phan loai muc nhiet do hien tai thanh cac nhan de hien thi.
String DS18B20Sensor::getStatusString() {
    float temp = lastTemp;
    if (temp < 20.0) return "LANH";
    if (temp < 30.0) return "BINH THUONG";
    if (temp < 40.0) return "AM";
    return "NONG";
}
