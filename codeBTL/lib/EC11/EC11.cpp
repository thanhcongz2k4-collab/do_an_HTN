#include "EC11.h"

// Static instance cho ISR
EC11* EC11::_instance = nullptr;

EC11::EC11() {
    position = 0; // Vị trí hiện tại của encoder
    lastDirection = EC11_DIR_NONE; // trạng thái quay trước đó
    lastStateA = HIGH; // Trạng thái trước đó của chân A mặc dịnh là high do dùng pull-up
    lastButtonState = HIGH; // Trạng thái trước đó của nút nhấn (chưa nhấn)
    buttonState = HIGH; // Trạng thái hiện tại của nút nhấn high là chưa nhấn
    buttonPressed = false; // Trạng thái nhấn nút
    buttonHeld = false; // Trạng thái nhấn giữ nút 
    lastDebounceTime = 0; // Thời gian chống dội nút nhấn
    buttonPressTime = 0; // mốc thời gian bắt đầu nhấn nút
    minPos = 0;
    maxPos = 0;
    hasLimit = false;
}

void IRAM_ATTR EC11::isrHandler() {
    if (_instance) _instance->handleEncoder();
}

void IRAM_ATTR EC11::handleEncoder() {
    bool currentA = digitalRead(EC11_CLK_PIN); // Đọc trạng thái hiện tại của chân A
    if (currentA != lastStateA && currentA == LOW) {
        bool currentB = digitalRead(EC11_DT_PIN); // Đọc trạng thái hiện tại của chân B tại thời điểm cạnh A để xác định hướng quay
        if (currentB != currentA) {
            position++;
            lastDirection = EC11_DIR_CW;
        } else {
            position--;
            lastDirection = EC11_DIR_CCW;
        }
        if (hasLimit) {
            if (position < minPos) position = minPos;
            if (position > maxPos) position = maxPos;
        }
    }
    lastStateA = currentA;
}

void EC11::begin() { // hàm bắt đầu chạy của EC11 
    pinMode(EC11_CLK_PIN, INPUT_PULLUP); // chân này là để bắt xung quay
    pinMode(EC11_DT_PIN, INPUT_PULLUP); //chân này để biết quay trái hay phải 
    pinMode(EC11_SW_PIN, INPUT_PULLUP); // chân này là để nhấn nút nếu chưa nhấn là mức cao nhấn là mức thấp
    lastStateA = digitalRead(EC11_CLK_PIN); // đọc trạng thái hiện tại của chân A để lưu vào trạng thái cuối cùng

    _instance = this;
    attachInterrupt(digitalPinToInterrupt(EC11_CLK_PIN), isrHandler, CHANGE);
}

void EC11::update() {
    // Chi xu ly nut nhan (polling), xoay da xu ly bang ngat
    bool reading = digitalRead(EC11_SW_PIN);
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
        if (reading != buttonState) {
            buttonState = reading;
            if (buttonState == LOW) { // hiểu là nút vừa bấm xuống
                buttonPressTime = millis();
            } else {
                if (!buttonHeld) {
                    buttonPressed = true; //tính là nhấn ngắn hợp lệ
                }
                buttonHeld = false;
            }
        }
        if (buttonState == LOW && !buttonHeld) { // Nếu nút đang ở trạng thái nhấn và chưa được đánh dấu là giữ, thì vào kiểm tra 
            if ((millis() - buttonPressTime) >= HOLD_TIME) {
                buttonHeld = true; 
            }
        }
    }

    lastButtonState = reading;
}

int EC11::getPosition() { return position; }

void EC11::setPosition(int pos) {
    position = pos;
    if (hasLimit) {
        if (position < minPos) position = minPos;
        if (position > maxPos) position = maxPos;
    }
}

void EC11::setLimit(int minVal, int maxVal) {
    minPos = minVal;
    maxPos = maxVal;
    hasLimit = true;
    if (position < minPos) position = minPos;
    if (position > maxPos) position = maxPos;
}

void EC11::removeLimit() { hasLimit = false; }

int EC11::getDirection() {
    int dir = lastDirection;
    lastDirection = EC11_DIR_NONE;
    return dir;
}

bool EC11::isTurnedCW() {
    if (lastDirection == EC11_DIR_CW) {
        lastDirection = EC11_DIR_NONE;
        return true;
    }
    return false;
}

bool EC11::isTurnedCCW() {
    if (lastDirection == EC11_DIR_CCW) {
        lastDirection = EC11_DIR_NONE;
        return true;
    }
    return false;
}

bool EC11::isTurned() {
    if (lastDirection != EC11_DIR_NONE) { // 
        lastDirection = EC11_DIR_NONE;
        return true;
    }
    return false;
}

bool EC11::isButtonPressed() {
    if (buttonPressed) {
        buttonPressed = false;
        return true;
    }
    return false;
}

bool EC11::isButtonHeld() {
    if (buttonHeld) {
        buttonHeld = false;
        return true;
    }
    return false;
}

bool EC11::isButtonDown() {
    return (buttonState == LOW);
}
