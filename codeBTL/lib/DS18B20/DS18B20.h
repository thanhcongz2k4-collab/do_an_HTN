#ifndef DS18B20_H
#define DS18B20_H

#include "define.h"
#include <OneWire.h>
#include <DallasTemperature.h>

class DS18B20Sensor {
private:
    OneWire oneWire;
    DallasTemperature sensors;
    float lastTemp;             // Lưu giá trị nhiệt độ gần nhất

public:
    DS18B20Sensor();
    
    // Khởi tạo cảm biến
    void begin();
    
    // Đọc nhiệt độ (°C)
    float readTempC();
    
    // Đọc nhiệt độ (°F)
    float readTempF();
    
    // Lấy giá trị nhiệt độ lần đọc gần nhất (không đọc lại)
    float getLastTemp();
    
    // Kiểm tra cảm biến có kết nối không
    bool isConnected();
    
    // Lấy trạng thái nhiệt độ dạng chuỗi
    String getStatusString();
};

#endif
