#include "EC11.h"

// Static instance cho ISR
EC11* EC11::_instance = nullptr;

EC11::EC11() {
    position = 0;
    lastDirection = EC11_DIR_NONE;
    lastStateA = HIGH;
    lastButtonState = HIGH;
    buttonState = HIGH;
    buttonPressed = false;
    buttonHeld = false;
    lastDebounceTime = 0;
    buttonPressTime = 0;
    minPos = 0;
    maxPos = 0;
    hasLimit = false;
}

void IRAM_ATTR EC11::isrHandler() {
    if (_instance) _instance->handleEncoder();
}

void IRAM_ATTR EC11::handleEncoder() {
    bool currentA = digitalRead(EC11_CLK_PIN);
    if (currentA != lastStateA && currentA == LOW) {
        bool currentB = digitalRead(EC11_DT_PIN);
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

void EC11::begin() {
    pinMode(EC11_CLK_PIN, INPUT_PULLUP);
    pinMode(EC11_DT_PIN, INPUT_PULLUP);
    pinMode(EC11_SW_PIN, INPUT_PULLUP);
    lastStateA = digitalRead(EC11_CLK_PIN);

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
            if (buttonState == LOW) {
                buttonPressTime = millis();
            } else {
                if (!buttonHeld) {
                    buttonPressed = true;
                }
                buttonHeld = false;
            }
        }
        if (buttonState == LOW && !buttonHeld) {
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
    if (lastDirection != EC11_DIR_NONE) {
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
