#include "buzzer.h"

Buzzer::Buzzer(uint8_t buzzerPin, bool activeLow) {
    pin = buzzerPin;
    isActiveLow = activeLow;
}

void Buzzer::begin() {
    pinMode(pin, OUTPUT);
    off();
}
void Buzzer::on() {
    if (isActiveLow) {
        digitalWrite(pin, LOW);
    } else {
        digitalWrite(pin, HIGH);
    }
}

void Buzzer::off() {
    if (isActiveLow) {
        digitalWrite(pin, HIGH);
    } else {
        digitalWrite(pin, LOW);
    }
}

void Buzzer::beep(unsigned long duration) {
    on();
    vTaskDelay(pdMS_TO_TICKS(duration));
    off();
}

void Buzzer::alarmBeep() {
    // Bíp bíp 2 lần rồi dừng
    on();
    vTaskDelay(pdMS_TO_TICKS(150));
    off();
    vTaskDelay(pdMS_TO_TICKS(100));
    on();
    vTaskDelay(pdMS_TO_TICKS(150));
    off();
}

void Buzzer::setActiveLow(bool activeLow) {
    isActiveLow = activeLow;
    off();
}