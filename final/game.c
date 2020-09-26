#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "ball.h"

#define HEIGHT 5

/** Define PIO pins driving LED matrix rows.  */
static const pio_t rows[] =
{
    LEDMAT_ROW1_PIO, LEDMAT_ROW2_PIO, LEDMAT_ROW3_PIO,
    LEDMAT_ROW4_PIO, LEDMAT_ROW5_PIO, LEDMAT_ROW6_PIO,
    LEDMAT_ROW7_PIO
};

/** Define PIO pins driving LED matrix columns.  */
static const pio_t cols[] =
{
    LEDMAT_COL1_PIO, LEDMAT_COL2_PIO, LEDMAT_COL3_PIO,
    LEDMAT_COL4_PIO, LEDMAT_COL5_PIO
};

static pio_t prev;
static void display_column (uint8_t row_pattern, uint8_t current_column)
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


int main (void)
{
    system_init ();
    uint8_t current_column = 0;
    pacer_init(500);

    /* Initialise LED matrix pins.  */
    for (int i = 0; i < 5; i++) {
        pio_config_set(cols[i], PIO_OUTPUT_HIGH);
    }
    for (int i = 0; i < 7; i++) {
        pio_config_set(rows[i], PIO_OUTPUT_HIGH);
    }

    Ball ball = {3,4,-1,-1};
    uint8_t bitmap[5] = {0};
    get_bitmap(bitmap, ball);


    while (1)
    {
        for (int i = 0; i < 200; i++) {
            pacer_wait ();

            display_column (bitmap[current_column], current_column);

            current_column++;

            if (current_column > (LEDMAT_COLS_NUM - 1)) {
                current_column = 0;
            }
        }
        update_location(&ball, 3);
        get_bitmap(bitmap, ball);
    }
}
