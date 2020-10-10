/** @file paddle.c
 * @author Emma Hogan, Tom Rizzi
 * @date 26 September 2020
 * @brief paddle behaviour module
 * last edited 10 October 2020 by Emma Hogan
 */


#include "paddle.h"


/** Returns the paddles current position:
    @param paddle pointer to paddle struct
    @return int of paddle position */
uint8_t get_paddle_location(Paddle* paddle) {
    return paddle->pos;
};


/** Initialise the paddle struct:
    @param paddle pointer to paddle struct */
void paddle_init(Paddle* paddle)
{
    paddle->pos = PADDLE_START_POS;
}


/** Returns a bitmap of the current paddle position:
    @param paddle pointer to paddle struct 
    @param bitmap bitmap array for ledmat*/
void get_paddle_bitmap(Paddle* paddle, uint8_t* bitmap)
{
    bitmap[PADDLE_COL] = 0;
    // Set the bit representing the middle of the paddle
    bitmap[PADDLE_COL] |= (1 << (PADDLE_LIMIT_RIGHT - paddle->pos));

    // Check if it's not at the left side of the screen
    if (paddle->pos > PADDLE_LIMIT_LEFT) {
        // Set the bit representing the left side of the paddle
        bitmap[PADDLE_COL] |= (1 << (PADDLE_LIMIT_RIGHT - paddle->pos + 1));
    }

    // Check if it's not at the right side of the screen
    if (paddle->pos < PADDLE_LIMIT_RIGHT) {
        // Set the bit representing the right side of the paddle
        bitmap[PADDLE_COL] |= (1 << (PADDLE_LIMIT_RIGHT - paddle->pos - 1));
    }
}


/** Move the paddle one unit left if possible:
    @param paddle pointer to paddle struct */
void paddle_move_left(Paddle* paddle)
{
    // Check that the paddle isn't at the left limit
    if (paddle->pos > PADDLE_LIMIT_LEFT) {
        paddle->pos--;
    }
}


/** Move the paddle one unit right if possible:
    @param paddle pointer to paddle struct */
void paddle_move_right(Paddle* paddle)
{
    // Check the paddle isn't at the right limit
    if (paddle->pos < PADDLE_LIMIT_RIGHT) {
        paddle->pos++;
    }
}
