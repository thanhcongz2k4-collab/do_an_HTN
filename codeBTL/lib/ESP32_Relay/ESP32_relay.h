#ifndef ESP32_RELAY_H
#define ESP32_RELAY_H

#include "define.h"

class RelayController {
public:
    void begin();
    void update();

    void on(uint8_t index);
    void off(uint8_t index);
    void toggle(uint8_t index);
    bool read(uint8_t index);

    void allOn();
    void allOff();

private:
    uint8_t _pins[NUM_RELAYS] = {RELAY_PIN_1, RELAY_PIN_2, RELAY_PIN_3, RELAY_PIN_4};
    bool _states[NUM_RELAYS] = {false};

    // Nut nhan tat ca (GPIO35)
    bool _lastReading = HIGH;
    bool _confirmed = HIGH;
    unsigned long _lastDebounce = 0;

    // 3 nut nhan rieng le (PULLUP)
    uint8_t _btnPins[NUM_EXTRA_BUTTONS]   = {BUTTON_2_PIN, BUTTON_3_PIN, BUTTON_4_PIN};
    uint8_t _btnRelay[NUM_EXTRA_BUTTONS]  = {3, 1, 2};  // D27->relay3(D19), D26->relay1(D17), D12->relay2(D18)
    bool _btnLast[NUM_EXTRA_BUTTONS]      = {HIGH, HIGH, HIGH};
    bool _btnConf[NUM_EXTRA_BUTTONS]      = {HIGH, HIGH, HIGH};
    unsigned long _btnDebounce[NUM_EXTRA_BUTTONS] = {0};
};

#endif