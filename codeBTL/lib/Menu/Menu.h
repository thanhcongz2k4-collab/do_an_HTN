#ifndef MENU_H
#define MENU_H

#include "define.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>
#include "EC11.h"

// Cau truc lich hen gio
struct Schedule {
    bool enabled;           // Bat/Tat
    uint8_t startHour;      // Gio bat dau
    uint8_t startMinute;    // Phut bat dau
    uint8_t endHour;        // Gio ket thuc (chi dung cho o cam)
    uint8_t endMinute;      // Phut ket thuc (chi dung cho o cam)
};

// Trang thai menu
enum MenuState {
    STATE_HOME,             // Man hinh chinh
    STATE_MAIN_MENU,        // Menu chinh
    STATE_DISPLAY_INFO,     // Hien thi nhiet do & muc nuoc
    STATE_SCHEDULE_LIST,    // Danh sach 4 lich hen gio
    STATE_SCHEDULE_EDIT,    // Chinh sua lich hen gio
    STATE_FEED_MANUAL       // Cho ca an thu cong
};

class Menu {
public:
    Menu();

    // Khoi tao OLED + EC11 (goi trong setup)
    void begin();

    // Cap nhat encoder + menu (goi trong loop)
    void update();

    // Luu/Doc du lieu lich tu flash
    void saveToFlash();
    void loadFromFlash();

    // Cap nhat du lieu cam bien
    void setTemperature(float temp);
    void setWaterLevel(int level);

    // Lay lich hen gio
    Schedule getOutletSchedule(uint8_t outlet, uint8_t schedIdx);
    Schedule getFeedSchedule(uint8_t schedIdx);

    // Kiem tra co yeu cau cho ca an thu cong
    bool isFeedManualTriggered();

    // Lay trang thai hien tai
    MenuState getState();

private:
    Adafruit_SSD1306 _display;
    EC11 _encoder;
    MenuState _state;

    // Dieu huong
    int _cursor;
    int _scrollOffset;
    bool _editing;          // Dang chinh sua gia tri

    // Du lieu lich
    Schedule _outletSched[MAX_OUTLETS][MAX_SCHEDULES];
    Schedule _feedSched[MAX_SCHEDULES];

    // Dang chinh sua
    uint8_t _selOutlet;     // 0-3: o cam, 4: cho ca an
    uint8_t _selSchedule;   // 0-3: lich 1-4
    Schedule _tempSched;    // Lich tam khi dang chinh sua

    // Du lieu cam bien
    float _temperature;
    int _waterLevel;

    // Co cho ca an thu cong
    bool _feedManualFlag;

    // Thoi gian ve lai
    unsigned long _lastDrawTime;

    // Xu ly trang thai
    void handleHome(int dir, bool press, bool hold);
    void handleMainMenu(int dir, bool press, bool hold);
    void handleDisplayInfo(int dir, bool press, bool hold);
    void handleScheduleList(int dir, bool press, bool hold);
    void handleScheduleEdit(int dir, bool press, bool hold);
    void handleFeedManual(int dir, bool press, bool hold);

    // Ve man hinh
    void drawHome();
    void drawMainMenu();
    void drawDisplayInfo();
    void drawScheduleList();
    void drawScheduleEdit();
    void drawFeedManual();

    // Tien ich
    int getEditFieldCount();
    void changeState(MenuState newState);
    void printTime2Digits(uint8_t val);
    void sortSchedules(Schedule *scheds, int count);

    Preferences _prefs;
};

#endif
