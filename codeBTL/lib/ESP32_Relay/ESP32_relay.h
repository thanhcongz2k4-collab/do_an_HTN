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

    // 4 nut nhan doc lap: D35, D27, D26, D34
    static const uint8_t NUM_BUTTONS = NUM_EXTRA_BUTTONS + 1;
    uint8_t _btnPins[NUM_BUTTONS]    = {BUTTON_PIN, BUTTON_2_PIN, BUTTON_3_PIN, BUTTON_4_PIN};
    uint8_t _btnRelay[NUM_BUTTONS]   = {0, 3, 1, 2};
    bool _btnLast[NUM_BUTTONS]       = {HIGH, HIGH, HIGH, HIGH}; //trạng thái nút ở lần quét trước
    bool _btnConf[NUM_BUTTONS]       = {HIGH, HIGH, HIGH, HIGH}; //trạng thái đã xác nhận ổn định sau debounce
    unsigned long _btnDebounce[NUM_BUTTONS] = {0, 0, 0, 0};
};

#endif