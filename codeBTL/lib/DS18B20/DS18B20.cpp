#include "DS18B20.h"

// Constructor - khởi tạo OneWire trên pin được chỉ định
DS18B20::DS18B20(int pin) : oneWire(pin), sensors(&oneWire), dataPin(pin), temperature(0.0) {
}

// Khởi tạo cảm biến
void DS18B20::begin() {
  sensors.begin();
  // Đặt độ phân giải của cảm biến (9-12 bit)
  // 12 bit = độ chính xác cao, thời gian đọc lâu hơn
  sensors.setResolution(12);
  
  Serial.print("DS18B20 initialized on pin ");
  Serial.print(dataPin);
  Serial.print(" - Found ");
  Serial.print(sensors.getDeviceCount());
  Serial.println(" sensor(s)");
}

// Yêu cầu đọc nhiệt độ (không chặn)
void DS18B20::requestTemperature() {
  sensors.requestTemperatures();
}

// Lấy nhiệt độ từ cảm biến đầu tiên (°C)
float DS18B20::getTemperature() {
  temperature = sensors.getTempCByIndex(0);
  
  // Kiểm tra lỗi đọc
  if (temperature == DEVICE_DISCONNECTED_C) {
    Serial.println("Error: DS18B20 disconnected or read failed!");
    return -127.0;
  }
  
  return temperature;
}

// Lấy số lượng cảm biến được kết nối
int DS18B20::getSensorCount() {
  return sensors.getDeviceCount();
}

// Lấy nhiệt độ từ địa chỉ cảm biến cụ thể
float DS18B20::getTemperatureByAddress(uint8_t* address) {
  float temp = sensors.getTempC(address);
  
  if (temp == DEVICE_DISCONNECTED_C) {
    Serial.println("Error: Could not read temperature from sensor!");
    return -127.0;
  }
  
  return temp;
}

// Tìm tất cả cảm biến được kết nối
void DS18B20::findAllSensors() {
  int deviceCount = sensors.getDeviceCount();
  
  Serial.print("Finding ");
  Serial.print(deviceCount);
  Serial.println(" DS18B20 sensor(s)...");
  
  uint8_t address[8];
  for (int i = 0; i < deviceCount; i++) {
    if (oneWire.search(address)) {
      Serial.print("Sensor ");
      Serial.print(i);
      Serial.print(" address: ");
      for (int j = 0; j < 8; j++) {
        if (address[j] < 0x10) Serial.print("0");
        Serial.print(address[j], HEX);
        if (j < 7) Serial.print(":");
      }
      Serial.println();
    }
  }
  oneWire.reset_search();
}

// In thông tin debug
void DS18B20::printDebugInfo() {
  Serial.println("=== DS18B20 Debug Info ===");
  Serial.print("Data Pin: ");
  Serial.println(dataPin);
  Serial.print("Connected Sensors: ");
  Serial.println(sensors.getDeviceCount());
  Serial.print("Last Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.println("========================");
}
