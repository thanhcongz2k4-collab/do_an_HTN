#ifndef WATER_H
#define WATER_H

#include "define.h"

class WaterSensor {
private:
    int minValue;       // Giá trị ADC tối thiểu (không có nước)
    int maxValue;       // Giá trị ADC tối đa (ngập hoàn toàn)

public:
    WaterSensor();
    void begin();
    
    // Cấu hình calibration
    void setCalibration(int minVal, int maxVal);
    
    // Đọc giá trị phần trăm mực nước (0-100%)
    int readPercent();
    
    // Đọc giá trị ADC thô (debug)
    int readRaw();
    
    // Kiểm tra trạng thái mức nước
    bool isLow();               // Mức nước thấp (< 50%)
    bool isMedium();            // Mức nước trung bình (50% - < 80%)
    bool isFull();              // Mức nước đầy (>= 80%)
    
    // Lấy trạng thái dạng chuỗi
    String getStatusString();
};

#endif
