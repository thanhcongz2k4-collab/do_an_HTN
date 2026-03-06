#include "ESP32_relay.h"

void RelayController::begin() {
    for (int i = 0; i < NUM_RELAYS; i++) {
        pinMode(_pins[i], OUTPUT);
        digitalWrite(_pins[i], LOW); // OFF (active LOW)
    }
    // GPIO35 khong co internal pull-up -> dung INPUT + dien tro pull-up ngoai 10kΩ
    pinMode(BUTTON_PIN, INPUT);

    // 3 nut nhan moi: INPUT_PULLUP
    for (int i = 0; i < NUM_EXTRA_BUTTONS; i++) {
        pinMode(_btnPins[i], INPUT_PULLUP);
    }
}

void RelayController::update() {
    // === Nut nhan tat ca (GPIO35) ===
    bool reading = digitalRead(BUTTON_PIN);

    if (reading != _lastReading) {
        _lastDebounce = millis();
    }

    if ((millis() - _lastDebounce) > DEBOUNCE_MS) {
        if (reading != _confirmed) {
            _confirmed = reading;
            if (_confirmed == LOW) {
                toggle(0);  // D35 chi dieu khien relay D16 (index 0)
            }
        }
    }

    _lastReading = reading;

    // === 3 nut nhan rieng le ===
    for (int i = 0; i < NUM_EXTRA_BUTTONS; i++) {
        bool rd = digitalRead(_btnPins[i]);

        if (rd != _btnLast[i]) {
            _btnDebounce[i] = millis();
        }

        if ((millis() - _btnDebounce[i]) > DEBOUNCE_MS) {
            if (rd != _btnConf[i]) {
                _btnConf[i] = rd;
                if (_btnConf[i] == LOW) {
                    toggle(_btnRelay[i]);
                }
            }
        }

        _btnLast[i] = rd;
    }
}

void RelayController::on(uint8_t index) {
    if (index >= NUM_RELAYS) return;
    _states[index] = true;
    digitalWrite(_pins[index], HIGH);
}

void RelayController::off(uint8_t index) {
    if (index >= NUM_RELAYS) return;
    _states[index] = false;
    digitalWrite(_pins[index], LOW);
}

void RelayController::toggle(uint8_t index) {
    if (index >= NUM_RELAYS) return;
    _states[index] ? off(index) : on(index);
}

bool RelayController::read(uint8_t index) {
    if (index >= NUM_RELAYS) return false;
    return _states[index];
}

void RelayController::allOn() {
    for (int i = 0; i < NUM_RELAYS; i++) on(i);
}

void RelayController::allOff() {
    for (int i = 0; i < NUM_RELAYS; i++) off(i);
}