#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "define.h"
#include <WiFi.h>
#include <time.h>
#include "Menu.h"
#include "ESP32_relay.h"
#include "ESP32_Servo.h"

class ScheduleManager {
public:
    ScheduleManager();

    // Khoi tao WiFi, NTP, relay, servo (goi trong setup)
    void begin(const char* ssid, const char* password);

    // Kiem tra lich va dieu khien thiet bi (goi trong loop)
    void update(Menu &menu);

    // Lay thoi gian hien tai tu NTP
    uint8_t getHour();
    uint8_t getMinute();
    uint8_t getSecond();

    // Kiem tra WiFi ket noi
    bool isWiFiConnected();

    // Kiem tra da dong bo thoi gian NTP chua
    bool isTimeSynced();

    // Tham chieu relay/servo de main co the dieu khien truc tiep neu can
    RelayController& getRelays();
    ESP32_Servo& getServo();

private:
    RelayController _relays;
    ESP32_Servo _servo;

    uint8_t _currentHour;
    uint8_t _currentMinute;

    unsigned long _lastCheck;
    bool _timeSynced;

    // Cap nhat thoi gian tu NTP
    bool updateTime();

    // Kiem tra thoi gian co nam trong khoang [start, end) khong
    bool isInRange(uint8_t curH, uint8_t curM,
                   uint8_t startH, uint8_t startM,
                   uint8_t endH, uint8_t endM);

    // Kiem tra thoi gian khop chinh xac (cho ca an)
    bool isExactTime(uint8_t curH, uint8_t curM,
                     uint8_t targetH, uint8_t targetM);

    // Co tranh cho ca an nhieu lan trong 1 phut
    bool _fedThisMinute[MAX_SCHEDULES];
    uint8_t _lastFedMinute;

    // Theo doi trang thai lich truoc do, chi dieu khien relay khi lich THAY DOI
    int8_t _lastSchedShouldOn[MAX_OUTLETS]; // -1: chua xac dinh, 0: lich muon tat, 1: lich muon bat
};

#endif
