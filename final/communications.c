/** @file communications.c
 * @author Emma Hogan, Tom Rizzi
 * @date 9 October 2020
 * @brief IR communications module
 * last edited 9 October 2020 by Emma Hogan
 */

#include "communications.h"


/** transmit relevant ball information:
    @param ball struct containing ball data*/
void transmit_ball (Ball* ball)
{
    if (!ball->dead) {
        // note that x direction and coord must mirror current direction and coord because fun kits are facing eachother
        uint8_t x_coord = RIGHT_WALL - ball->x;
        int8_t x_dir = -1 * ball->direction_x;

        // enforce encodings to be strictly positive
        uint8_t encoded_x_coord = encode(x_coord + COORD_OFFSET);
        uint8_t encoded_x_dir = encode(x_dir + DIR_OFFSET);

        ir_uart_putc(encoded_x_coord);
        ir_uart_putc(encoded_x_dir);

    } else { //ball just died, only need to transmit deadness
        uint8_t encoded_message = encode(DEAD_BALL);
        ir_uart_putc(encoded_message);
    }
}


/** inform other microcontroller that game has been started
    @param mode 1 for start game, 2 for start round*/
void inform_start (uint8_t mode)
{
    uint8_t val = encode(mode);
    ir_uart_putc(val);
}


/** Receive relevant ball information from other device
    @param ball struct containing ball data*/
void receive_ball (Ball* ball)
{
    uint8_t encoded_x_coord;
    uint8_t encoded_x_dir;
    if (ir_uart_read_ready_p()) {
        encoded_x_coord = ir_uart_getc();
        uint8_t x_coord = decode(encoded_x_coord) - COORD_OFFSET;
        if (x_coord <= RIGHT_WALL) { //we are receiving a transmission of ball location
            encoded_x_dir = ir_uart_getc();
            int8_t x_dir = decode(encoded_x_dir) - DIR_OFFSET;

            ball->x = x_coord;
            if (x_dir < LEFT || x_dir > RIGHT) {
                //transmission got messed up, default to straight down
                ball->direction_x = STRAIGHT;
            } else {
                ball->direction_x = x_dir;
            }

            //init y values
            ball->y = HEIGHT - 1;
            ball->direction_y = DOWN;

            //set to on screen
            ball->on_screen = 1;
        } else if (x_coord + COORD_OFFSET == DEAD_BALL) { //we are being told the ball is dead
            ball->dead = 1;
        }
    }
}
