#ifndef DS18B20_H
#define DS18B20_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class DS18B20 {
  private:
    OneWire oneWire;
    DallasTemperature sensors;
    int dataPin;
    float temperature;

  public:
    // Constructor
    DS18B20(int pin);

    // Khởi tạo cảm biến
    void begin();

    // Yêu cầu đọc nhiệt độ
    void requestTemperature();

    // Lấy nhiệt độ (°C)
    float getTemperature();

    // Lấy số lượng cảm biến được kết nối
    int getSensorCount();

    // Lấy nhiệt độ từ địa chỉ cụ thể
    float getTemperatureByAddress(uint8_t* address);

    // Tìm tất cả địa chỉ cảm biến
    void findAllSensors();

    // In thông tin cảm biến ra Serial (để debug)
    void printDebugInfo();
};

#endif
