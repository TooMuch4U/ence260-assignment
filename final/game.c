#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "ball.h"
#include "coder.h"
#include "paddle.h"
#include "navswitch.h"
#include "ir_uart.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"

#define HEIGHT 5
#define COORD_OFFSET 1
#define DIR_OFFSET 2
#define PACER_RATE 500
#define MESSAGE_RATE 10

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

static void inform_start (uint8_t mode)
{
    uint8_t val = encode(mode);
    ir_uart_putc(val);
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


static void run_start_menu (void)
{
    // scroll the start of game text until one player starts paddle screen
    uint8_t display_mode = 0;
    while (!display_mode) {
        pacer_wait ();
        tinygl_update();
        // Check for navswitch presses
        navswitch_update();

        // Check for a push
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            display_mode = 1;
            inform_start(1); //tell other controller a game has been started
        }
        // Check if the other fun kit pressed start
        if (ir_uart_read_ready_p()) {
            uint8_t val = ir_uart_getc();
            uint8_t decoded_val = decode(val);
            if (decoded_val == 1) { //we are receiving a transmission, not noise
                display_mode = 1;
            }
        }
    }
}

static void move_paddle (void)
{
    // Check for navswitch presses
    navswitch_update();

    //Check for a left push
    if (navswitch_push_event_p(NAVSWITCH_SOUTH)) {
        paddle_move_left();
    }

    // Check for a right push
    if (navswitch_push_event_p(NAVSWITCH_NORTH)) {
        paddle_move_right();
    }
}


static uint8_t update_display (uint8_t bitmap[], uint8_t current_column)
{
    // Update the display
    get_paddle_bitmap(bitmap);
    display_column (bitmap[current_column], current_column);

    // Update column
    current_column++;
    if (current_column > (LEDMAT_COLS_NUM - 1)) {
        current_column = 0;
    }
    return current_column;
}


static uint8_t run_paddle_only (uint8_t bitmap[])
{
    //run game with paddle only until one user fires a ball
    uint8_t ball_fired = 0;
    uint8_t game_mode;
    uint8_t current_column = 0;
    while (!ball_fired) {
        pacer_wait ();
        
        //listen for move paddle instructions
        move_paddle();
        current_column = update_display(bitmap, current_column);
        
        // Check for a push
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            // release the kraken
            ball_fired = 1; 
            inform_start(2); //tell other controller a ball has been released
            game_mode = 2; //we are the one firing the ball
        }
        
         // Check if the other fun kit pressed start
        if (ir_uart_read_ready_p()) {
            uint8_t val = ir_uart_getc();
            uint8_t decoded_val = decode(val);
            if (decoded_val == 2) { //we are receiving a transmission, not noise
                ball_fired = 1;
                game_mode = 1; //we are waiting on a ball to receive
            }
        }
    }
    return game_mode;
}


int main (void)
{
    system_init ();
    uint8_t current_column = 0;
    uint8_t bitmap[5] = {0};
    pacer_init(PACER_RATE);
    navswitch_init();
    ir_uart_init();

    tinygl_init (PACER_RATE);
    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    tinygl_text ("PONG ");
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);

    /* Initialise LED matrix pins.  */
    for (int i = 0; i < 5; i++) {
        pio_config_set(cols[i], PIO_OUTPUT_HIGH);
    }
    for (int i = 0; i < 7; i++) {
        pio_config_set(rows[i], PIO_OUTPUT_HIGH);
    }

    paddle_init();
    run_start_menu(); //will wait until game is being started
    uint8_t game_state = run_paddle_only(bitmap); //display paddle only until someone fires a ball
    Ball ball;
    if (game_state == 1) {
        //other player is starting. Initialise ball to be offscreen and wait for transmission
        ball_init(&ball, 0, 5, 0, 0, 0);
    } else {
        //we just started the game. Shoot a ball
        uint8_t paddle_loc = get_paddle_location();
        ball_init(&ball, paddle_loc, 1, 0, 1, 1);
    }
    
    get_bitmap(bitmap, ball);
    get_paddle_bitmap(bitmap);
    uint8_t updateBallCount = 0;
    while (1) {
        pacer_wait ();
        move_paddle();
        current_column = update_display(bitmap, current_column);

        updateBallCount++;
        //if the ball is on screen and the timer is right, update location
        if (ball.on_screen) {
            if (updateBallCount > 100) {
                updateBallCount = 0;
                update_location(&ball, get_paddle_location());
                get_bitmap(bitmap, ball);

                if (!ball.on_screen) {
                    //if ball just moved off screen, transmit relevant info
                    transmit_ball(&ball);
                }
            }
        } else {
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
