#include "buzzer.h"

// Khoi tao buzzer voi chan dieu khien va kieu kich muc logic.
// activeLow=true: keo LOW thi buzzer keu, activeLow=false: keo HIGH thi buzzer keu.
Buzzer::Buzzer(uint8_t buzzerPin, bool activeLow) {
    pin = buzzerPin;
    isActiveLow = activeLow;
}

// Cau hinh chan output va dua buzzer ve trang thai tat an toan luc khoi dong.
void Buzzer::begin() {
    pinMode(pin, OUTPUT);
    off();
}

// Bat buzzer theo dung cuc tinh da cau hinh.
void Buzzer::on() {
    if (isActiveLow) {
        digitalWrite(pin, LOW);
    } else {
        digitalWrite(pin, HIGH);
    }
}

// Tat buzzer theo dung cuc tinh da cau hinh.
void Buzzer::off() {
    if (isActiveLow) {
        digitalWrite(pin, HIGH);
    } else {
        digitalWrite(pin, LOW);
    }
}

// Keu buzzer trong mot khoang thoi gian xac dinh (ms), ham se block task hien tai.
void Buzzer::beep(unsigned long duration) {
    on();
    vTaskDelay(pdMS_TO_TICKS(duration));
    off();
}

// Phat am thanh canh bao ngan theo mau 2 tieng beep.
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

// Doi cuc tinh active-low/active-high trong runtime va dong thiet bi ngay lap tuc.
void Buzzer::setActiveLow(bool activeLow) {
    isActiveLow = activeLow;
    off();
}