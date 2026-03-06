#ifndef DEFINE_H
#define DEFINE_H

#include <Arduino.h>

// ============================================================
//  DEBUG - Comment dong duoi de tat log Serial
// ============================================================
// #define DEBUG_PRINT

#ifdef DEBUG_PRINT
  #define DEBUG_LOG(fmt, ...)   Serial.printf(fmt, ##__VA_ARGS__)
  #define DEBUG_LOGLN(msg)      Serial.println(msg)
#else
  #define DEBUG_LOG(fmt, ...)
  #define DEBUG_LOGLN(msg)
#endif

// ============================================================
//  DINH NGHIA CHAN (PIN)
// ============================================================

// --- EC11 Rotary Encoder ---
#define EC11_CLK_PIN        33
#define EC11_DT_PIN         32
#define EC11_SW_PIN         25

// --- DS18B20 Cam bien nhiet do ---
#define DS18B20_PIN         4

// --- Cam bien muc nuoc (ADC1) ---
#define WATER_SENSOR_PIN    36

// --- Relay ---
#define RELAY_PIN_1         15
#define RELAY_PIN_2         16
#define RELAY_PIN_3         17
#define RELAY_PIN_4         18
#define NUM_RELAYS          4
#define BUTTON_PIN          35
#define BUTTON_2_PIN        27    // Nut nhan dieu khien relay D19
#define BUTTON_3_PIN        26    // Nut nhan dieu khien relay D17
#define BUTTON_4_PIN        34    // Nut nhan dieu khien relay D18
#define NUM_EXTRA_BUTTONS   3
#define DEBOUNCE_MS         10

// --- Servo cho ca an ---
#define SERVO_PIN           13
#define SERVO_CHANNEL       0
#define SERVO_FREQ          50
#define SERVO_RES           16
#define SERVO_MIN_US        500
#define SERVO_MAX_US        2500
#define FEED_ANGLE          180
#define CLOSE_ANGLE         0
#define FEED_DURATION       1000

// --- Buzzer ---
#define BUZZER_PIN          5

// --- OLED ---
#define SCREEN_WIDTH        128
#define SCREEN_HEIGHT       64
#define OLED_ADDRESS        0x3C

// ============================================================
//  HANG SO HE THONG
// ============================================================

// EC11
#define EC11_DIR_NONE       0
#define EC11_DIR_CW         1
#define EC11_DIR_CCW        -1

// Muc nuoc
#define WATER_LEVEL_LOW     50
#define WATER_LEVEL_MEDIUM  80
#define WATER_LEVEL_FULL    100

// Menu
#define MAX_OUTLETS         4
#define MAX_SCHEDULES       4
#define MAIN_MENU_COUNT     7
#define VISIBLE_ITEMS       6

// Schedule / NTP
#define SCHEDULE_CHECK_INTERVAL  1000
#define NTP_SERVER          "pool.ntp.org"
#define GMT_OFFSET          25200
#define DST_OFFSET          0

// ============================================================
//  CAU HINH WIFI
// ============================================================
#define WIFI_SSID           "Phòng 603"
#define WIFI_PASSWORD       "88888888"

#endif