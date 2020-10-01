#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "ball.h"
#include "coder.h"
#include "paddle.h"
#include "navswitch.h"
#include "ir_uart.h"

#define HEIGHT 5
#define COORD_OFFSET 1
#define DIR_OFFSET 2

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

static void transmit_ball (Ball* ball)
{
    // note that x direction and coord must mirror current direction and coord because fun kits are facing eachother
    int x_coord = RIGHT_WALL - ball->x;
    int x_dir = -1 * ball->direction_x; 
    
    // enforce encodings to be strictly positive
    int encoded_x_coord = encode(x_coord + COORD_OFFSET);
    int encoded_x_dir = encode(x_dir + DIR_OFFSET);
    
    ir_uart_putc(encoded_x_coord);
    ir_uart_putc(encoded_x_dir);
}

static void receive_ball (Ball* ball)
{
    int encoded_x_coord;
    int encoded_x_dir;
    if (ir_uart_read_ready_p()) {
	    encoded_x_coord = ir_uart_getc();
        int x_coord = decode(encoded_x_coord) - COORD_OFFSET;
        if (x_coord >= LEFT_WALL && x_coord <= RIGHT_WALL) { //we are receiving a transmission, not noise
            encoded_x_dir = ir_uart_getc();
            int x_dir = decode(encoded_x_dir) - DIR_OFFSET;
            
            ball->x = x_coord;
            ball->direction_x = x_dir;
            
            //init y values
            ball->y = HEIGHT - 1;
            ball->direction_y = DOWN;
            
            //set to on screen
            ball->on_screen = 1;
        }
    }
}
    


int main (void)
{
    system_init ();
    uint8_t current_column = 0;
    pacer_init(500);
    navswitch_init();
    ir_uart_init();

    /* Initialise LED matrix pins.  */
    for (int i = 0; i < 5; i++) {
        pio_config_set(cols[i], PIO_OUTPUT_HIGH);
    }
    for (int i = 0; i < 7; i++) {
        pio_config_set(rows[i], PIO_OUTPUT_HIGH);
    }

    //set to x2 = 5 and x5 = 0 for funkit with ball starting offscreen
    //set to x2 < 5 and x5 = 1 for funkit with ball starting onscreen
    //eventually this will be determined by who starts the game
    Ball ball = {3,4,0,-1,1};
    uint8_t bitmap[5] = {0};
    paddle_init();
    get_bitmap(bitmap, ball);
    get_paddle_bitmap(bitmap);

    uint8_t updateBallCount = 0;

    while (1)
    {
        pacer_wait ();

        // Check for navswitch presses
        navswitch_update();
        // Check for a left push
        if (navswitch_push_event_p(NAVSWITCH_SOUTH)) {
            paddle_move_left();
        }

        // Check for a right push
        if (navswitch_push_event_p(NAVSWITCH_NORTH)) {
            paddle_move_right();
        }

        // Update the paddle on display
        get_paddle_bitmap(bitmap);
        display_column (bitmap[current_column], current_column);

        // Update column
        current_column++;
        if (current_column > (LEDMAT_COLS_NUM - 1)) {
            current_column = 0;
        }

        
        updateBallCount++;
        //if the ball is on screen and the timer is right, update location
        if (ball.on_screen) {
            if (updateBallCount > 200) {
                updateBallCount = 0;
                update_location(&ball, get_paddle_location());
                get_bitmap(bitmap, ball);
                
                if (!ball.on_screen) {
                    //if ball just moved off screen, transmit relevant info
                    transmit_ball(&ball);
                }
            }
        }
        else {
            //listen constantly for transmissions while the ball is offscreen
            receive_ball(&ball);
            if (ball.on_screen) {
                //reset ball timer
                updateBallCount = 0;
                get_bitmap(bitmap, ball);
            }
        }
    }
}
