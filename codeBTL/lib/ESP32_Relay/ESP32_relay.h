#ifndef ESP32_RELAY_H
#define ESP32_RELAY_H

#include <Arduino.h>

class Relay {
public:
    Relay(uint8_t pin, bool activeLow = true);
    void begin();
    void on();
    void off();
    bool read();

private:
    uint8_t _pin;
    bool _activeLow;
};

#endif
