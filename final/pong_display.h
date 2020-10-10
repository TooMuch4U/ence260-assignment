/** @file pong_display.h
 * @author Emma Hogan, Tom Rizzi
 * @date 26 September 2020
 * @brief ledmat screen display module
 * last edited 11 October 2020 by Emma Hogan
 */


#ifndef PONG_DISPLAY_H
#define PONG_DISPLAY_H

#include "pong_display.h"
#include "pio.h"
#include "ir_uart.h"
#include "pacer.h"
#include "tinygl.h"
#include "system.h"
#include "../fonts/font5x7_1.h"
#include "../fonts/font3x5_1.h"

#define PACER_RATE 600
#define MESSAGE_RATE 10


/** Initialise the columns of the led matrix: */
void init_led_matrix (void);


/** Flash the correct bit pattern for current column in led matrix:
    @param row_pattern the bitmap of which leds we want to light
    @param current_column, the index of the column we are currently flashing */
void display_column (uint8_t row_pattern, uint8_t current_column);


/** Update the led matrix to display given bitmap, flashing 1 column at a time:
    @param bitmap the pattern we want to light up
    @param current_column the column we are currently flashing
    @return an integer representing the next column to flash */
uint8_t update_display (uint8_t bitmap[], uint8_t current_column);


/** Setup tinygl to display given text in scrolling mode:
    @param text, the characters to display */
void scroll_text (char* text);


/** Flash a single character onto the screen:
    @param character, the character to display */
void display_character (char character);


#endif //ASSIGMENT_DISPLAY_H
