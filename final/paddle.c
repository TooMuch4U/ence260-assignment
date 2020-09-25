//
// Created by tcr59 on 26/09/20.
//

#include "paddle.h"

/**
 * Structure for paddle
 */
typedef struct {
    uint8_t pos;
} Paddle;

/**
 * Initialise paddle
 */
void paddle_init()
{
    PADDLE_INSTANCE = {PADDLE_START_POS};
}

/**
 * Returns a bitmap of the current paddle position
 */
int get_paddle_bitmap(PADDLE_START_POS)
{
    return 0
}

/**
 * Checks for movement of navswitch, moves paddle accordingly
 */
void paddle_move_check()
{

    // Check for a left push
    if (navswitch_push_event_p(NAVSWITCH_EAST)) {
        paddle_move_left();
    }

    // Check for a right push
    if (navswitch_push_event_p(NAVSWITCH_WEST)) {
        paddle_move_right();
    }
}

/**
 * Move the paddle to the left once.
 * Will only move if the paddle isn't already at the limit
 */
void paddle_move_left()
{
    // Check that the paddle isn't at the far limit yet
    if (PADDLE_INSTANCE.pos > PADDLE_LEFT_LIMIT) {
        PADDLE_INSTANCE.pos--;
    }
}

/**
 * Moves the paddle once to the right.
 * Only moves the paddle if the paddle isn't already at the right limit
 */
void paddle_move_right()
{
    // Check the paddle isn't at the right limit
    if (PADDLE_INSTANCE.pos < PADDLE_RIGHT_LIMIT) {
        PADDLE_INSTANCE.pos++;
    }
}