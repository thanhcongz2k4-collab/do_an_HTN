#ifndef BUZZER_H
#define BUZZER_H

#include "define.h"

class Buzzer {
private:
    uint8_t pin;
    bool isActiveLow;  // true: LOW = kêu, false: HIGH = kêu

public:
    Buzzer(uint8_t buzzerPin = BUZZER_PIN, bool activeLow = true);
    
    void begin();
    void on();
    void off();
    
    // Kêu 1 tiếng bíp
    void beep(unsigned long duration = 200);
    
    // Kêu bíp bíp 2 lần cảnh báo rồi tự dừng
    void alarmBeep();
    
    void setActiveLow(bool activeLow);
};

#endif