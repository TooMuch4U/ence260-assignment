//
// Created by tcr59 on 26/09/20.
// Last edited by ech105 on 8/10/20
//

#include "pong_display.h"


/** Define PIO pins driving LED matrix rows.  */
static const pio_t rows[] = {
    LEDMAT_ROW1_PIO, LEDMAT_ROW2_PIO, LEDMAT_ROW3_PIO,
    LEDMAT_ROW4_PIO, LEDMAT_ROW5_PIO, LEDMAT_ROW6_PIO,
    LEDMAT_ROW7_PIO
};


/** Define PIO pins driving LED matrix columns.  */
static const pio_t cols[] = {
    LEDMAT_COL1_PIO, LEDMAT_COL2_PIO, LEDMAT_COL3_PIO,
    LEDMAT_COL4_PIO, LEDMAT_COL5_PIO
};

/** initialise the columns of the led matrix */
void init_led_matrix(void)
{
    /* Initialise LED matrix pins.  */
    for (int i = 0; i < 5; i++) {
        pio_config_set(cols[i], PIO_OUTPUT_HIGH);
    }
    for (int i = 0; i < 7; i++) {
        pio_config_set(rows[i], PIO_OUTPUT_HIGH);
    }
}

static pio_t prev;
/** Flash the correct bit pattern for current column in led matrix */
void display_column (uint8_t row_pattern, uint8_t current_column)
{
    pio_output_high(cols[prev]);
    prev = current_column;
    for (int current_row = 0; current_row < 7; current_row++) {
        if ((row_pattern >> current_row) & 1) {
            pio_output_low(rows[current_row]);
        } else {
            pio_output_high(rows[current_row]);
        }
    }
    //change after updating rows to prevent ghosting
    pio_output_low(cols[current_column]);
}


/** update the led matrix to display given bitmap, flashing 1 column at a time */
uint8_t update_display (uint8_t bitmap[], uint8_t current_column)
{
    // Update the display
    display_column (bitmap[current_column], current_column);

    // Update column
    current_column++;
    if (current_column > (LEDMAT_COLS_NUM - 1)) {
        current_column = 0;
    }
    return current_column;
}


/** setup tinygl to display given text in scrolling mode */
void scroll_text(char* text)
{
    tinygl_init (PACER_RATE);
    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    tinygl_text (text);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
}

/** flash a single character onto the screen for a small amount of time */
void display_character (char character)
{
    char buffer[2];
    buffer[0] = character;
    buffer[1] = '\0';
    tinygl_text_mode_set (TINYGL_TEXT_MODE_STEP); //kind of flash onto screen
    tinygl_text (buffer);

    for (int i = 0; i < 500; i++) {
        pacer_wait();
        tinygl_update();
    }
}
