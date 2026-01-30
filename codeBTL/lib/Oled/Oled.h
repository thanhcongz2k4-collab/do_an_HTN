#ifndef OLED_H
#define OLED_H

#include <Arduino.h>

void oled_init();
void oled_print(const char* text);
void oled_print_xy(uint8_t x, uint8_t y, const char* text);
void oled_clear();

#endif
