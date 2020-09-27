//
// Created by tcr59 on 26/09/20.
//

#include "paddle.h"

// Paddle instance
static Paddle PADDLE_INSTANCE;

/**
 * Initialise paddle
 */
void paddle_init(void)
{
    PADDLE_INSTANCE.pos = PADDLE_START_POS;
}

/**
 * Returns a bitmap of the current paddle position
 */
void get_paddle_bitmap(uint8_t* bitmap)
{
    bitmap[4] |= (1 << (6-PADDLE_INSTANCE.pos));

    // Check if it's at the left side of the screen
    if (PADDLE_INSTANCE.pos > 0) {
        bitmap[4] |= (1 << (7-PADDLE_INSTANCE.pos));
    }

    // Check if it's at the right side of the screen
    if (PADDLE_INSTANCE.pos < 6) {
        bitmap[4] |= (1 << (5-PADDLE_INSTANCE.pos));
    }
}

/**
 * Checks for movement of navswitch, moves paddle accordingly
 */
void paddle_move_check(void)
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
void paddle_move_left(void)
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
void paddle_move_right(void)
{
    // Check the paddle isn't at the right limit
    if (PADDLE_INSTANCE.pos < PADDLE_RIGHT_LIMIT) {
        PADDLE_INSTANCE.pos++;
    }
}
