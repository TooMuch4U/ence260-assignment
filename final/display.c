//
// Created by tcr59 on 26/09/20.
//

#include "display.h"

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

void update_paddle()
{
}

/**
 * Initilise display
 */
void display_init()
{
    // Initilise leds
    for (int i = 0; i < 7; i++) {
        if (i < 5) {
            pio_config_set(cols[i], PIO_OUTPUT_HIGH);
        }
        pio_config_set(rows[i], PIO_OUTPUT_HIGH);
    }
}

void display_column()
{

}

void display()
{

}