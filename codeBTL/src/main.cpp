#include "define.h"
#include <WiFi.h>
#include "Menu.h"
#include "schedule.h"
#include "DS18B20.h"
#include "water.h"
#include "buzzer.h"

// Khai bao doi tuong
Menu menu;
ScheduleManager scheduler;
DS18B20Sensor tempSensor;
WaterSensor waterSensor;
Buzzer buzzer(BUZZER_PIN, false);

// Mutex bao ve truy cap menu (lich, cam bien, co cho ca an)
SemaphoreHandle_t menuMutex;

// ===== TASK 1: Relay nut nhan + Lap lich (uu tien cao, 20ms) =====
void taskRelaySchedule(void* param);
void taskSensors(void* param);
void taskDisplay(void* param);

void setup() {
    Serial.begin(115200);

    menuMutex = xSemaphoreCreateMutex();

    menu.begin();
    Wire.setClock(400000);  // I2C Fast Mode 400kHz cho OLED
    menu.loadFromFlash();
    scheduler.begin(WIFI_SSID, WIFI_PASSWORD);
    tempSensor.begin();
    waterSensor.begin();
    waterSensor.setCalibration(0, 2100, 2650);
    buzzer.begin();
    pinMode(L298_IN1_PIN, OUTPUT);
    Serial.println(F("He thong be ca thong minh da khoi dong!"));

    // Task relay+lich: uu tien 3 (cao nhat), core 1
    xTaskCreatePinnedToCore(taskRelaySchedule, "Relay", 4096, NULL, 3, NULL, 1);

    // Task cam bien+coi: uu tien 1 (thap), core 1
    xTaskCreatePinnedToCore(taskSensors, "Sensor", 4096, NULL, 1, NULL, 1);

    // Task OLED+EC11: uu tien 2, core 0
    xTaskCreatePinnedToCore(taskDisplay, "Display", 4096, NULL, 2, NULL, 0);
}

void loop() {
    // loop() ranh - tat ca da chay trong FreeRTOS tasks
    vTaskDelay(pdMS_TO_TICKS(1000));
}

// ===== TASK 1: Relay nut nhan + Lap lich (uu tien cao, 20ms) =====
void taskRelaySchedule(void* param) {
    for (;;) {
        // Nut nhan relay - KHONG can mutex, chi doc/ghi GPIO
        scheduler.getRelays().update();

        // Kiem tra lich - can mutex de doc lich tu menu
        if (xSemaphoreTake(menuMutex, pdMS_TO_TICKS(5))) {
            scheduler.update(menu);
            xSemaphoreGive(menuMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(1)); // 20ms loop -> nut nhan phan hoi nhanh
    }
}

// ===== TASK 2: Doc cam bien + Coi canh bao (2 giay) =====
void taskSensors(void* param) {
    for (;;) {
        float temp = tempSensor.readTempC();
        int waterRaw = waterSensor.readRaw();
        int water = waterSensor.readPercent();
        //Serial.printf("Water RAW: %d, Percent: %d%%\n", waterRaw, water);

        // Cap nhat gia tri len menu
        if (xSemaphoreTake(menuMutex, pdMS_TO_TICKS(10))) {
            menu.setTemperature(temp);
            menu.setWaterLevel(water);
            xSemaphoreGive(menuMutex);
        }

        // Canh bao nhiet do bat thuong
        if (temp < 20 || temp > 30) {
            buzzer.alarmBeep();
            digitalWrite(L298_IN1_PIN , HIGH); // Bat quat
            Serial.println("Nhiet do bat thuong! Bat quat lam mat.");
        }
        else {
            digitalWrite(L298_IN1_PIN , LOW); // Tat quat
            Serial.println("Nhiet do binh thuong. Tat quat.");
        }

        // Canh bao muc nuoc thap
        if (waterSensor.isLow()) {
            buzzer.alarmBeep();
        }

        vTaskDelay(pdMS_TO_TICKS(2000)); // Doc moi 2 giay
    }
}

// ===== TASK 3: OLED + Encoder EC11 (50ms) =====
void taskDisplay(void* param) {
    for (;;) {
        if (xSemaphoreTake(menuMutex, pdMS_TO_TICKS(10))) {
            menu.update();
            xSemaphoreGive(menuMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(50)); // 50ms -> man hinh muot, encoder nhay
    }
}
