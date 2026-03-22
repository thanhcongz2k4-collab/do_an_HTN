#ifndef EC11_H
#define EC11_H

#include "define.h"

class EC11 {
private:
    // Xoay encoder (xu ly trong ISR)
    volatile int position;
    volatile int lastDirection;
    volatile bool lastStateA;

    // Nut nhan (polling trong update)
    bool lastButtonState;
    bool buttonState;
    volatile bool buttonPressed;
    volatile bool buttonHeld;
    unsigned long lastDebounceTime;
    unsigned long buttonPressTime;
    static const unsigned long DEBOUNCE_DELAY = 20;
    static const unsigned long HOLD_TIME = 500;

    // Gioi han vi tri
    int minPos;
    int maxPos;
    bool hasLimit;

    // ISR handler
    static EC11* _instance;
    static void IRAM_ATTR isrHandler();
    void IRAM_ATTR handleEncoder();

public:
    EC11();
    void begin();
    void update();  // Chi xu ly nut nhan
    int getDirection();  
    bool isButtonPressed();
    bool isButtonHeld();
};

#endif
