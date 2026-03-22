#include "Menu.h"

// Nhan menu chinh
static const char* mainMenuLabels[] = {
    "Nhiet do & Muc nuoc",
    "Lap lich o cam 1",
    "Lap lich o cam 2",
    "Lap lich o cam 3",
    "Lap lich o cam 4",
    "Lap lich cho ca an",
    "Cho ca an thu cong"
};

Menu::Menu()
    : _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1) {
    _state = STATE_HOME;
    _cursor = 0;
    _scrollOffset = 0;
    _editing = false;
    _selOutlet = 0;
    _selSchedule = 0;
    _temperature = 0.0;
    _waterLevel = 0;
    _feedManualFlag = false;
    _lastDrawTime = 0;
    memset(_outletSched, 0, sizeof(_outletSched));
    memset(_feedSched, 0, sizeof(_feedSched));
}

// Khoi tao OLED, encoder va ve giao dien trang chu ban dau.
void Menu::begin() {
    // Khoi tao OLED
    if (!_display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        Serial.println(F("SSD1306 loi!"));
        for (;;);
    }
    _display.clearDisplay();
    _display.setTextSize(1);
    _display.setTextColor(SSD1306_WHITE);
    _display.cp437(true);

    // Khoi tao encoder
    _encoder.begin();

    drawHome();
}

// Vong cap nhat UI: doc input encoder/nut, xu ly state machine va redraw khi can.
void Menu::update() {
    // Doc encoder
    _encoder.update();
    int direction = _encoder.getDirection();
    bool pressed = _encoder.isButtonPressed();
    bool held = _encoder.isButtonHeld();

    // Xu ly input theo trang thai
    switch (_state) {
        case STATE_HOME:          handleHome(direction, pressed, held); break;
        case STATE_MAIN_MENU:     handleMainMenu(direction, pressed, held); break;
        case STATE_DISPLAY_INFO:  handleDisplayInfo(direction, pressed, held); break;
        case STATE_SCHEDULE_LIST: handleScheduleList(direction, pressed, held); break;
        case STATE_SCHEDULE_EDIT: handleScheduleEdit(direction, pressed, held); break;
        case STATE_FEED_MANUAL:   handleFeedManual(direction, pressed, held); break;
    }

    // Ve lai man hinh khi co input hoac moi 250ms (cap nhat du lieu song)
    bool hasInput = (direction != 0 || pressed || held);
    unsigned long now = millis();
    if (hasInput || (now - _lastDrawTime > 250)) {
        _lastDrawTime = now;
        switch (_state) {
            case STATE_HOME:          drawHome(); break;
            case STATE_MAIN_MENU:     drawMainMenu(); break;
            case STATE_DISPLAY_INFO:  drawDisplayInfo(); break;
            case STATE_SCHEDULE_LIST: drawScheduleList(); break;
            case STATE_SCHEDULE_EDIT: drawScheduleEdit(); break;
            case STATE_FEED_MANUAL:   drawFeedManual(); break;
        }
    }
}

// Chuyen trang thai man hinh va reset bo dem dieu huong de vao trang moi sach se.
void Menu::changeState(MenuState newState) {
    _state = newState;
    _cursor = 0;
    _scrollOffset = 0;
    _editing = false;
}

// ===================== XU LY TRANG THAI =====================

// Trang HOME: nhan nut de vao menu chinh.
void Menu::handleHome(int dir, bool press, bool hold) {
    if (press) {
        changeState(STATE_MAIN_MENU);
    }
}

// Xu ly dieu huong danh sach menu chinh va mo chuc nang tuong ung.
void Menu::handleMainMenu(int dir, bool press, bool hold) {
    if (hold) {
        changeState(STATE_HOME);
        return;
    }
    if (dir != 0) {
        _cursor += dir;
        if (_cursor < 0) _cursor = 0;
        if (_cursor >= MAIN_MENU_COUNT) _cursor = MAIN_MENU_COUNT - 1;
        // Cuon danh sach neu can
        if (_cursor < _scrollOffset) _scrollOffset = _cursor;
        if (_cursor >= _scrollOffset + VISIBLE_ITEMS)
            _scrollOffset = _cursor - VISIBLE_ITEMS + 1;
    }
    if (press) {
        if (_cursor == 0) {
            // Hien thi nhiet do & muc nuoc
            changeState(STATE_DISPLAY_INFO);
        } else if (_cursor >= 1 && _cursor <= 4) {
            // Lap lich o cam 1-4
            _selOutlet = _cursor - 1;
            changeState(STATE_SCHEDULE_LIST);
        } else if (_cursor == 5) {
            // Lap lich cho ca an
            _selOutlet = 4;
            changeState(STATE_SCHEDULE_LIST);
        } else if (_cursor == 6) {
            // Cho ca an thu cong
            changeState(STATE_FEED_MANUAL);
        }
    }
}

// Trang thong tin cam bien: nhan hoac giu nut de quay lai menu chinh.
void Menu::handleDisplayInfo(int dir, bool press, bool hold) {
    if (hold || press) {
        changeState(STATE_MAIN_MENU);
        _cursor = 0;
    }
}

// Chon mot lich trong danh sach lich cua o cam hoac lich cho an.
void Menu::handleScheduleList(int dir, bool press, bool hold) {
    if (hold) {
        // Quay lai menu chinh, giu vi tri con tro
        int savedOutlet = _selOutlet;
        changeState(STATE_MAIN_MENU);
        _cursor = (savedOutlet < 4) ? (savedOutlet + 1) : 5;
        if (_cursor >= VISIBLE_ITEMS)
            _scrollOffset = _cursor - VISIBLE_ITEMS + 1;
        return;
    }
    if (dir != 0) {
        _cursor += dir;
        if (_cursor < 0) _cursor = 0;
        if (_cursor >= MAX_SCHEDULES) _cursor = MAX_SCHEDULES - 1;
    }
    if (press) {
        _selSchedule = _cursor;
        // Sao chep lich vao bien tam de chinh sua
        if (_selOutlet < 4) {
            _tempSched = _outletSched[_selOutlet][_selSchedule];
        } else {
            _tempSched = _feedSched[_selSchedule];
        }
        changeState(STATE_SCHEDULE_EDIT);
    }
}

// Chinh sua chi tiet mot lich: di chuyen field, doi gia tri, luu vao flash.
void Menu::handleScheduleEdit(int dir, bool press, bool hold) {
    int fieldCount = getEditFieldCount();

    if (hold) {
        // Thoat khong luu
        int savedSchedule = _selSchedule;
        changeState(STATE_SCHEDULE_LIST);
        _cursor = savedSchedule;
        return;
    }

    if (_editing) {
        // Dang chinh sua: xoay de thay doi gia tri
        if (dir != 0) {
            if (_selOutlet < 4) {
                // O cam
                switch (_cursor) {
                    case 0: _tempSched.enabled = !_tempSched.enabled; break;
                    case 1: _tempSched.startHour = (_tempSched.startHour + dir + 24) % 24; break;
                    case 2: _tempSched.startMinute = (_tempSched.startMinute + dir + 60) % 60; break;
                    case 3: _tempSched.endHour = (_tempSched.endHour + dir + 24) % 24; break;
                    case 4: _tempSched.endMinute = (_tempSched.endMinute + dir + 60) % 60; break;
                }
            } else {
                // Cho ca an
                switch (_cursor) {
                    case 0: _tempSched.enabled = !_tempSched.enabled; break;
                    case 1: _tempSched.startHour = (_tempSched.startHour + dir + 24) % 24; break;
                    case 2: _tempSched.startMinute = (_tempSched.startMinute + dir + 60) % 60; break;
                }
            }
        }
        if (press) {
            _editing = false;  // Xac nhan gia tri
        }
    } else {
        // Di chuyen con tro giua cac truong
        if (dir != 0) {
            _cursor += dir;
            if (_cursor < 0) _cursor = 0;
            if (_cursor >= fieldCount) _cursor = fieldCount - 1;
        }
        if (press) {
            if (_cursor == fieldCount - 1) {
                // Nhan [LUU] => Luu va thoat
                if (_selOutlet < 4) {
                    _outletSched[_selOutlet][_selSchedule] = _tempSched;
                    sortSchedules(_outletSched[_selOutlet], MAX_SCHEDULES);
                } else {
                    _feedSched[_selSchedule] = _tempSched;
                    sortSchedules(_feedSched, MAX_SCHEDULES);
                }
                saveToFlash();
                int savedSchedule = _selSchedule;
                changeState(STATE_SCHEDULE_LIST);
                _cursor = savedSchedule;
            } else if (_cursor == 0) {
                // Truong Bat/Tat: toggle truc tiep
                _tempSched.enabled = !_tempSched.enabled;
            } else {
                // Truong gio/phut: vao che do chinh sua
                _editing = true;
            }
        }
    }
}

// Trang cho an thu cong: nhan de kich hoat co feed, giu de thoat.
void Menu::handleFeedManual(int dir, bool press, bool hold) {
    if (hold) {
        changeState(STATE_MAIN_MENU);
        _cursor = 6;
        if (_cursor >= VISIBLE_ITEMS)
            _scrollOffset = _cursor - VISIBLE_ITEMS + 1;
        return;
    }
    if (press) {
        _feedManualFlag = true;
    }
}

// ===================== VE MAN HINH =====================

// In gia tri thoi gian theo dang 2 chu so (vd: 03, 12).
void Menu::printTime2Digits(uint8_t val) {
    if (val < 10) _display.print('0');
    _display.print(val);
}

// Ve man hinh tong quan he thong (nhiet do, muc nuoc, huong dan vao menu).
void Menu::drawHome() {
    _display.clearDisplay();
    _display.setTextSize(1);
    _display.setTextColor(SSD1306_WHITE);

    _display.setCursor(10, 0);
    _display.print(F("BE CA THONG MINH"));
    _display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

    // Nhiet do
    _display.setCursor(0, 16);
    _display.print(F("Nhiet do: "));
    _display.print(_temperature, 1);
    _display.print(F(" C"));

    // Muc nuoc
    _display.setCursor(0, 28);
    _display.print(F("Muc nuoc: "));
    _display.print(_waterLevel);
    _display.print(F("%"));

    // Huong dan
    _display.setCursor(0, 56);
    _display.print(F("  Nhan nut -> Menu"));

    _display.display();
}

// Ve danh sach menu chinh kem con tro va mui ten cuon.
void Menu::drawMainMenu() {
    _display.clearDisplay();
    _display.setTextSize(1);
    _display.setTextColor(SSD1306_WHITE);

    // Tieu de
    _display.setCursor(22, 0);
    _display.print(F("== MENU =="));

    // Danh sach menu
    for (int i = 0; i < VISIBLE_ITEMS && (i + _scrollOffset) < MAIN_MENU_COUNT; i++) {
        int idx = i + _scrollOffset;
        _display.setCursor(0, 10 + i * 9);
        _display.print((idx == _cursor) ? F("> ") : F("  "));
        _display.print(mainMenuLabels[idx]);
    }

    // Mui ten cuon len
    if (_scrollOffset > 0) {
        _display.fillTriangle(124, 11, 120, 16, 127, 16, SSD1306_WHITE);
    }
    // Mui ten cuon xuong
    if (_scrollOffset + VISIBLE_ITEMS < MAIN_MENU_COUNT) {
        _display.fillTriangle(124, 63, 120, 58, 127, 58, SSD1306_WHITE);
    }

    _display.display();
}

// Ve man hinh thong tin chi tiet nhiet do va muc nuoc.
void Menu::drawDisplayInfo() {
    _display.clearDisplay();
    _display.setTextSize(1);
    _display.setTextColor(SSD1306_WHITE);

    _display.setCursor(28, 0);
    _display.print(F("THONG TIN"));
    _display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

    _display.setCursor(0, 16);
    _display.print(F("Nhiet do : "));
    _display.print(_temperature, 1);
    _display.print(F(" C"));

    _display.setCursor(0, 28);
    _display.print(F("Muc nuoc : "));
    _display.print(_waterLevel);
    _display.print(F("%"));

    // Trang thai nhiet do
    _display.setCursor(0, 36);
    _display.print(F("TT nhiet do: "));
    if (_temperature < 24) _display.print(F("LANH"));
    else if (_temperature <= 30) _display.print(F("BT"));
    else _display.print(F("AM"));

    // Trang thai muc nuoc
    _display.setCursor(0, 46);
    _display.print(F("TT muc nuoc: "));
    if (_waterLevel < 50) _display.print(F("THAP"));
    else if (_waterLevel < 80) _display.print(F("TB"));
    else _display.print(F("DAY"));

    _display.setCursor(0, 56);
    _display.print(F("Nhan/giu nut->Thoat"));

    _display.display();
}

// Ve danh sach toi da 4 lich cho doi tuong dang chon (o cam hoac cho an).
void Menu::drawScheduleList() {
    _display.clearDisplay();
    _display.setTextSize(1);
    _display.setTextColor(SSD1306_WHITE);

    // Tieu de
    _display.setCursor(0, 0);
    if (_selOutlet < 4) {
        _display.print(F("O CAM "));
        _display.print(_selOutlet + 1);
        _display.print(F(" - LAP LICH"));
    } else {
        _display.print(F("CHO CA AN-LAP LICH"));
    }
    _display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

    // Hien thi 4 lich
    for (int i = 0; i < MAX_SCHEDULES; i++) {
        Schedule *s = (_selOutlet < 4)
            ? &_outletSched[_selOutlet][i]
            : &_feedSched[i];

        _display.setCursor(0, 14 + i * 11);
        _display.print((i == _cursor) ? F(">") : F(" "));
        _display.print(i + 1);
        _display.print(F(":"));
        _display.print(s->enabled ? F("BAT ") : F("TAT "));

        printTime2Digits(s->startHour);
        _display.print(F(":"));
        printTime2Digits(s->startMinute);

        if (_selOutlet < 4) {
            _display.print(F("-"));
            printTime2Digits(s->endHour);
            _display.print(F(":"));
            printTime2Digits(s->endMinute);
        }
    }

    _display.setCursor(0, 56);
    _display.print(F("  Giu nut de thoat"));

    _display.display();
}

// Ve man hinh chinh sua 1 lich, co nhap nhay truong dang duoc sua.
void Menu::drawScheduleEdit() {
    _display.clearDisplay();
    _display.setTextSize(1);
    _display.setTextColor(SSD1306_WHITE);

    // Nhap nhay gia tri khi dang chinh sua
    bool blink = (millis() / 300) % 2;

    // Tieu de
    _display.setCursor(0, 0);
    if (_selOutlet < 4) {
        _display.print(F("O CAM "));
        _display.print(_selOutlet + 1);
    } else {
        _display.print(F("CHO CA AN"));
    }
    _display.print(F(" - LICH "));
    _display.print(_selSchedule + 1);

    int y, field;

    if (_selOutlet < 4) {
        // ===== O CAM: 6 truong (enabled, gBat, pBat, gTat, pTat, luu) =====
        y = 10; field = 0;

        // Trang thai
        _display.setCursor(0, y);
        _display.print((_cursor == field) ? ((_editing) ? F("*") : F(">")) : F(" "));
        _display.print(F("Trang thai: "));
        _display.print(_tempSched.enabled ? F("BAT") : F("TAT"));

        y += 9; field++;
        // Gio bat
        _display.setCursor(0, y);
        _display.print((_cursor == field) ? ((_editing) ? F("*") : F(">")) : F(" "));
        _display.print(F("Gio bat  : "));
        if (!(_editing && _cursor == field && blink))
            printTime2Digits(_tempSched.startHour);
        else
            _display.print(F("  "));

        y += 9; field++;
        // Phut bat
        _display.setCursor(0, y);
        _display.print((_cursor == field) ? ((_editing) ? F("*") : F(">")) : F(" "));
        _display.print(F("Phut bat : "));
        if (!(_editing && _cursor == field && blink))
            printTime2Digits(_tempSched.startMinute);
        else
            _display.print(F("  "));

        y += 9; field++;
        // Gio tat
        _display.setCursor(0, y);
        _display.print((_cursor == field) ? ((_editing) ? F("*") : F(">")) : F(" "));
        _display.print(F("Gio tat  : "));
        if (!(_editing && _cursor == field && blink))
            printTime2Digits(_tempSched.endHour);
        else
            _display.print(F("  "));

        y += 9; field++;
        // Phut tat
        _display.setCursor(0, y);
        _display.print((_cursor == field) ? ((_editing) ? F("*") : F(">")) : F(" "));
        _display.print(F("Phut tat : "));
        if (!(_editing && _cursor == field && blink))
            printTime2Digits(_tempSched.endMinute);
        else
            _display.print(F("  "));

        y += 9; field++;
        // Luu
        _display.setCursor(0, y);
        _display.print((_cursor == field) ? F(">") : F(" "));
        _display.print(F("[LUU]"));

    } else {
        // ===== CHO CA AN: 4 truong (enabled, gio, phut, luu) =====
        y = 14; field = 0;

        // Trang thai
        _display.setCursor(0, y);
        _display.print((_cursor == field) ? ((_editing) ? F("*") : F(">")) : F(" "));
        _display.print(F("Trang thai: "));
        _display.print(_tempSched.enabled ? F("BAT") : F("TAT"));

        y += 12; field++;
        // Gio
        _display.setCursor(0, y);
        _display.print((_cursor == field) ? ((_editing) ? F("*") : F(">")) : F(" "));
        _display.print(F("Gio      : "));
        if (!(_editing && _cursor == field && blink))
            printTime2Digits(_tempSched.startHour);
        else
            _display.print(F("  "));

        y += 12; field++;
        // Phut
        _display.setCursor(0, y);
        _display.print((_cursor == field) ? ((_editing) ? F("*") : F(">")) : F(" "));
        _display.print(F("Phut     : "));
        if (!(_editing && _cursor == field && blink))
            printTime2Digits(_tempSched.startMinute);
        else
            _display.print(F("  "));

        y += 14; field++;
        // Luu
        _display.setCursor(0, y);
        _display.print((_cursor == field) ? F(">") : F(" "));
        _display.print(F("[LUU]"));
    }

    _display.display();
}

// Ve giao dien cho an thu cong va trang thai da kich hoat.
void Menu::drawFeedManual() {
    _display.clearDisplay();
    _display.setTextSize(1);
    _display.setTextColor(SSD1306_WHITE);

    _display.setCursor(3, 0);
    _display.print(F("CHO CA AN THU CONG"));
    _display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

    if (_feedManualFlag) {
        _display.setCursor(16, 28);
        _display.print(F("Da cho ca an!"));
    } else {
        _display.setCursor(4, 28);
        _display.print(F("Nhan nut de cho an"));
    }

    _display.setCursor(4, 56);
    _display.print(F("Giu nut de thoat"));

    _display.display();
}

// ===================== GETTER / SETTER =====================

// Cap nhat gia tri nhiet do de cac man hinh hien thi thong tin moi.
void Menu::setTemperature(float temp) { _temperature = temp; }
// Cap nhat phan tram muc nuoc de cac man hinh hien thi thong tin moi.
void Menu::setWaterLevel(int level) { _waterLevel = level; }

// Lay 1 lich cua o cam theo chi so; neu sai pham vi thi tra ve lich rong.
Schedule Menu::getOutletSchedule(uint8_t outlet, uint8_t schedIdx) {
    if (outlet < MAX_OUTLETS && schedIdx < MAX_SCHEDULES)
        return _outletSched[outlet][schedIdx];
    return {false, 0, 0, 0, 0};
}

// Lay 1 lich cho an theo chi so; neu sai pham vi thi tra ve lich rong.
Schedule Menu::getFeedSchedule(uint8_t schedIdx) {
    if (schedIdx < MAX_SCHEDULES)
        return _feedSched[schedIdx];
    return {false, 0, 0, 0, 0};
}

// Tra ve co cho an thu cong theo kieu consume-on-read (doc xong se reset).
bool Menu::isFeedManualTriggered() {
    if (_feedManualFlag) {
        _feedManualFlag = false;
        return true;
    }
    return false;
}

// Tra ve trang thai man hinh hien tai cua state machine.
MenuState Menu::getState() { return _state; }

// So truong can sua phu thuoc loai lich: o cam (6) hoac cho an (4).
int Menu::getEditFieldCount() {
    return (_selOutlet < 4) ? 6 : 4;
}

// Sap xep lich theo gio bat tang dan; lich tat se day xuong cuoi danh sach.
void Menu::sortSchedules(Schedule *scheds, int count) {
    // Sap xep theo thoi gian bat dau tang dan
    // Lich TAT (enabled=false) day xuong cuoi
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            bool swapNeeded = false;
            if (!scheds[i].enabled && scheds[j].enabled) {
                // Lich tat xuong cuoi
                swapNeeded = true;
            } else if (scheds[i].enabled && scheds[j].enabled) {
                // Ca 2 deu bat: so sanh thoi gian
                uint16_t timeI = scheds[i].startHour * 60 + scheds[i].startMinute;
                uint16_t timeJ = scheds[j].startHour * 60 + scheds[j].startMinute;
                if (timeJ < timeI) swapNeeded = true;
            }
            if (swapNeeded) {
                Schedule tmp = scheds[i];
                scheds[i] = scheds[j];
                scheds[j] = tmp;
            }
        }
    }
}

// Luu toan bo lich o cam va lich cho an vao NVS flash.
void Menu::saveToFlash() {
    _prefs.begin("schedules", false);
    _prefs.putBytes("outlet", _outletSched, sizeof(_outletSched));
    _prefs.putBytes("feed", _feedSched, sizeof(_feedSched));
    _prefs.end();
}

// Nap lich tu NVS flash neu da ton tai key du lieu truoc do.
void Menu::loadFromFlash() {
    _prefs.begin("schedules", true);
    if (_prefs.isKey("outlet")) {
        _prefs.getBytes("outlet", _outletSched, sizeof(_outletSched));
    }
    if (_prefs.isKey("feed")) {
        _prefs.getBytes("feed", _feedSched, sizeof(_feedSched));
    }
    _prefs.end();
}
