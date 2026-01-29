#include <Arduino.h>
#include "ESP32_relay.h"

Relay::Relay(uint8_t pin, bool activeLow) {
    _pin = pin;
    _activeLow = activeLow;
}

void Relay::begin() {
    pinMode(_pin, OUTPUT);
    off();  // mặc định tắt relay
}
void Relay::on() {
    if (_activeLow)
        digitalWrite(_pin, LOW);
    else
        digitalWrite(_pin, HIGH);
}

void Relay::off() {
    if (_activeLow)
        digitalWrite(_pin, HIGH);
    else
        digitalWrite(_pin, LOW);
}

bool Relay::read() {
    int state = digitalRead(_pin);

    if (_activeLow)
        return (state == LOW);   // LOW = ON
    else
        return (state == HIGH);  // HIGH = ON
}
