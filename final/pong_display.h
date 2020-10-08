//
// Created by tcr59 on 26/09/20.
//

#ifndef ASSIGMENT_DISPLAY_H
#define ASSIGMENT_DISPLAY_H

#include "pong_display.h"
#include "pio.h"
#include "ir_uart.h"
#include "pacer.h"
#include "tinygl.h"
#include "system.h"
#include "../fonts/font5x7_1.h"

#define PACER_RATE 500
#define MESSAGE_RATE 10

void init_led_matrix(void);
void display_column (uint8_t row_pattern, uint8_t current_column);
uint8_t update_display (uint8_t bitmap[], uint8_t current_column);
void scroll_text(char* text);
void display_character (char character);

#endif //ASSIGMENT_DISPLAY_H
