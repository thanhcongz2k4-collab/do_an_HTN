#include "schedule.h"

// Khoi tao bien trang thai bo lap lich va cac co chong kich lai trong cung 1 phut.
ScheduleManager::ScheduleManager() {
    _currentHour = 0;
    _currentMinute = 0;
    _lastCheck = 0;
    _lastFedMinute = 255;
    _timeSynced = false;
    memset(_fedThisMinute, 0, sizeof(_fedThisMinute));
    memset(_lastSchedShouldOn, -1, sizeof(_lastSchedShouldOn));
}

// Khoi tao relay/servo, ket noi WiFi va dong bo thoi gian NTP de kich hoat lich.
void ScheduleManager::begin(const char* ssid, const char* password) {
    // Khoi tao relay va servo TRUOC WiFi de nut nhan hoat dong ngay
    _relays.begin();
    _servo.begin();

    // Ket noi WiFi
    Serial.print(F("Dang ket noi WiFi"));
    WiFi.begin(ssid, password);

    unsigned long startAttempt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 10000) {
        vTaskDelay(pdMS_TO_TICKS(500));
        Serial.print(".");
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.print(F("WiFi OK! IP: "));
        Serial.println(WiFi.localIP());

        // Cau hinh NTP voi nhieu server
        configTime(GMT_OFFSET, DST_OFFSET, NTP_SERVER, "time.google.com", "time.cloudflare.com");
        Serial.print(F("Dong bo NTP"));

        // Cho dong bo (timeout 1000ms moi lan, thu toi da 30 lan = 30 giay)
        struct tm timeinfo;
        int retry = 0;
        while (!getLocalTime(&timeinfo, 1000) && retry < 30) {
            Serial.print(".");
            retry++;
        }
        Serial.println();

        if (getLocalTime(&timeinfo, 100)) {
            _timeSynced = true;
            Serial.println(F("Dong bo thoi gian thanh cong!"));
            DEBUG_LOG("Gio: %02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        } else {
            Serial.println(F("NTP THAT BAI! Lich chua hoat dong."));
        }
    } else {
        Serial.println(F("WiFi THAT BAI! Lich chua hoat dong."));
    }
}

// Cap nhat gio/phut hien tai tu he thong (NTP da dong bo), khong block lau.
bool ScheduleManager::updateTime() { // cap nhat thoi gian hienej taij
    struct tm timeinfo;
    // Timeout 10ms - KHONG BLOCK, chi doc thoi gian da co san
    if (getLocalTime(&timeinfo, 10)) {
        _currentHour = timeinfo.tm_hour;
        _currentMinute = timeinfo.tm_min;
        _timeSynced = true;
        return true;
    }
    return false;
}

// Ham trung tam xu ly lich: cap nhat thoi gian, dieu khien relay, feed tu dong va feed thu cong.
void ScheduleManager::update(Menu &menu) { // cập nhật lịch từ menu và điều khiển thiết bị theo lịch
    unsigned long now = millis();
    if (now - _lastCheck < SCHEDULE_CHECK_INTERVAL) return;
    _lastCheck = now;

    // Doc thoi gian tu NTP
    if (!updateTime()) {
        // Thu dong bo lai NTP neu WiFi van ket noi
        if (WiFi.status() == WL_CONNECTED) {
            static unsigned long lastNtpRetry = 0;
            if (now - lastNtpRetry > 15000) {  // Thu lai moi 15 giay
                lastNtpRetry = now;
                configTime(GMT_OFFSET, DST_OFFSET, NTP_SERVER, "time.google.com", "time.cloudflare.com");
                DEBUG_LOGLN(F("[DEBUG] Dang thu dong bo lai NTP..."));
                // Cho 3 giay de NTP co thoi gian phan hoi
                struct tm timeinfo;
                if (getLocalTime(&timeinfo, 3000)) {
                    _timeSynced = true;
                    _currentHour = timeinfo.tm_hour;
                    _currentMinute = timeinfo.tm_min;
                    Serial.printf("NTP OK! Gio: %02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
                    return; // Da co thoi gian, tiep tuc xu ly lich o lan goi tiep
                } else {
                    DEBUG_LOGLN(F("[DEBUG] NTP van that bai!"));
                }
            }
        }
        return;
    }

    // In thoi gian moi giay
    static uint8_t lastPrintSec = 255;
    struct tm ti;
    if (getLocalTime(&ti, 10) && ti.tm_sec != lastPrintSec) {
        lastPrintSec = ti.tm_sec;
        DEBUG_LOG("[DEBUG] Gio hien tai: %02d:%02d:%02d\n", _currentHour, _currentMinute, ti.tm_sec);
    }

    // Reset co cho ca an khi sang phut moi
    if (_currentMinute != _lastFedMinute) {
        _lastFedMinute = _currentMinute;
        memset(_fedThisMinute, 0, sizeof(_fedThisMinute));
    }

    // ===== Kiem tra lich 4 o cam =====
    for (uint8_t i = 0; i < MAX_OUTLETS; i++) {
        bool hasSchedule = false;
        bool shouldOn = false;
        for (uint8_t j = 0; j < MAX_SCHEDULES; j++) {
            Schedule s = menu.getOutletSchedule(i, j);
            if (s.enabled) {
                hasSchedule = true;
                DEBUG_LOG("[DEBUG] Relay %d, Lich %d: BAT %02d:%02d - %02d:%02d\n",
                              i + 1, j + 1, s.startHour, s.startMinute, s.endHour, s.endMinute);
                bool inRange = isInRange(_currentHour, _currentMinute,
                              s.startHour, s.startMinute,
                              s.endHour, s.endMinute);
                DEBUG_LOG("[DEBUG]   -> inRange = %s\n", inRange ? "TRUE" : "FALSE");
                if (inRange) {
                    shouldOn = true;
                    break;
                }
            }
        }
        // Chi dieu khien relay khi TRANG THAI LICH THAY DOI
        // Cho phep nut nhan override giua cac lan chuyen lich
        if (hasSchedule) {
            int8_t newState = shouldOn ? 1 : 0;
            if (newState != _lastSchedShouldOn[i]) {
                _lastSchedShouldOn[i] = newState;
                if (shouldOn) {
                    DEBUG_LOG("[DEBUG] >>> Lich BAT Relay %d\n", i + 1);
                    _relays.on(i);
                } else {
                    DEBUG_LOG("[DEBUG] >>> Lich TAT Relay %d\n", i + 1);
                    _relays.off(i);
                }
            }
        }
    }

    // ===== Kiem tra lich cho ca an =====
    for (uint8_t j = 0; j < MAX_SCHEDULES; j++) {
        Schedule s = menu.getFeedSchedule(j);
        if (s.enabled && !_fedThisMinute[j] &&
            isExactTime(_currentHour, _currentMinute,
                        s.startHour, s.startMinute)) {
            _servo.feedFish();
            _fedThisMinute[j] = true;
        }
    }

    // ===== Cho ca an thu cong =====
    if (menu.isFeedManualTriggered()) {
        _servo.feedFish();
    }
}

// Kiem tra thoi diem hien tai co nam trong khoang [start, end) hay khong.
// Ho tro ca khoang cung ngay va khoang qua nua dem.
bool ScheduleManager::isInRange(uint8_t curH, uint8_t curM,
                                uint8_t startH, uint8_t startM,
                                uint8_t endH, uint8_t endM) {
    uint16_t cur   = curH * 60 + curM;
    uint16_t start = startH * 60 + startM;
    uint16_t end   = endH * 60 + endM;

    if (start == end) return false;

    if (start < end) {
        return (cur >= start && cur < end);
    } else {
        return (cur >= start || cur < end);
    }
}

// So khop chinh xac theo gio-phut, dung cho su kien cho an tai 1 moc cu the.
bool ScheduleManager::isExactTime(uint8_t curH, uint8_t curM,
                                  uint8_t targetH, uint8_t targetM) {
    return (curH == targetH && curM == targetM);
}

// Getter gio hien tai da cache.
uint8_t ScheduleManager::getHour()   { return _currentHour; }
// Getter phut hien tai da cache.
uint8_t ScheduleManager::getMinute() { return _currentMinute; }

// Lay giay hien tai truc tiep tu he thong de phuc vu hien thi/debug.
uint8_t ScheduleManager::getSecond() {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 10)) return timeinfo.tm_sec;
    return 0;
}

// Kiem tra trang thai ket noi WiFi hien tai.
bool ScheduleManager::isWiFiConnected() { return WiFi.status() == WL_CONNECTED; }
// Kiem tra he thong da dong bo thoi gian thanh cong hay chua.
bool ScheduleManager::isTimeSynced() { return _timeSynced; }

// Tra ve tham chieu bo dieu khien relay de task ben ngoai cap nhat nut nhan.
RelayController& ScheduleManager::getRelays() { return _relays; }
// Tra ve tham chieu servo cho an de su dung tu ben ngoai neu can.
ESP32_Servo& ScheduleManager::getServo() { return _servo; }
