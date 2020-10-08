//
// Created by tcr59 on 26/09/20.
//

#include "paddle.h"


/**
 * Returns the paddles current position
 * @return int of paddle position
 */
uint8_t get_paddle_location(Paddle* paddle) {
    return paddle->pos;
};

/**
 * Initialise paddle
 */
void paddle_init(Paddle* paddle)
{
    paddle->pos = PADDLE_START_POS;
}

/**
 * Returns a bitmap of the current paddle position
 */
void get_paddle_bitmap(Paddle* paddle, uint8_t* bitmap)
{
    bitmap[4] = 0;
    bitmap[4] |= (1 << (6-paddle->pos));

    // Check if it's at the left side of the screen
    if (paddle->pos > 0) {
        bitmap[4] |= (1 << (7 - paddle->pos));
    }

    // Check if it's at the right side of the screen
    if (paddle->pos < 6) {
        bitmap[4] |= (1 << (5 - paddle->pos));
    }
}

/**
 * Checks for movement of navswitch, moves paddle accordingly
 */
void paddle_move_check(Paddle* paddle)
{
    navswitch_update();
    // Check for a left push
    if (navswitch_push_event_p(NAVSWITCH_NORTH)) {
        paddle_move_left(paddle);
    }

    // Check for a right push
    if (navswitch_push_event_p(NAVSWITCH_SOUTH)) {
        paddle_move_right(paddle);
    }
}

/**
 * Move the paddle to the left once.
 * Will only move if the paddle isn't already at the limit
 */
void paddle_move_left(Paddle* paddle)
{
    // Check that the paddle isn't at the far limit yet
    if (paddle->pos > PADDLE_LEFT_LIMIT) {
        paddle->pos--;
    }
}

/**
 * Moves the paddle once to the right.
 * Only moves the paddle if the paddle isn't already at the right limit
 */
void paddle_move_right(Paddle* paddle)
{
    // Check the paddle isn't at the right limit
    if (paddle->pos < PADDLE_RIGHT_LIMIT) {
        paddle->pos++;
    }
}
