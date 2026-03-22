#include "ESP32_relay.h"
void RelayController::begin() {
    for (int i = 0; i < NUM_RELAYS; i++) {
        pinMode(_pins[i], OUTPUT);
        digitalWrite(_pins[i], LOW); // LOW là trạng thái OFF
    }
    // Ca 4 nut nhan deu dung INPUT + tro keo len ngoai 10k
    for (int i = 0; i < NUM_BUTTONS; i++) {
        pinMode(_btnPins[i], INPUT);
    }
}
// Quet trang thai nut nhan theo co che debounce va thuc hien toggle relay.
// Moi nut dieu khien mot relay doc lap theo bang anh xa _btnRelay.
void RelayController::update() {
    for (int i = 0; i < NUM_BUTTONS; i++) { //lap qua từng nút nhấn
        bool rd = digitalRead(_btnPins[i]); // lưu trạng thái hiện tại của nút thứ i
        if (rd != _btnLast[i]) {
            _btnDebounce[i] = millis(); // mốc thời gian lần đổi gần nhất của nút 
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

// Bat mot relay theo index va dong bo bien trang thai noi bo.
void RelayController::on(uint8_t index) {
    if (index >= NUM_RELAYS) return;
    _states[index] = true;
    digitalWrite(_pins[index], HIGH);
}

// Tat mot relay theo index va dong bo bien trang thai noi bo.
void RelayController::off(uint8_t index) {
    if (index >= NUM_RELAYS) return;
    _states[index] = false;
    digitalWrite(_pins[index], LOW);
}

// Dao trang thai relay: dang bat thi tat, dang tat thi bat.
void RelayController::toggle(uint8_t index) {
    if (index >= NUM_RELAYS) return;
    _states[index] ? off(index) : on(index);
}

// Doc trang thai relay dang duoc quan ly trong bo nho.
bool RelayController::read(uint8_t index) {
    if (index >= NUM_RELAYS) return false;
    return _states[index];
}

// Bat toan bo relay co san.
void RelayController::allOn() {
    for (int i = 0; i < NUM_RELAYS; i++) on(i);
}

// Tat toan bo relay co san.
void RelayController::allOff() {
    for (int i = 0; i < NUM_RELAYS; i++) off(i);
}