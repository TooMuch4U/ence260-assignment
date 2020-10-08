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
    //TODO GET RID OF THESE MAGIC NUMBERS
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

/** Move the paddle one unit left if possible:
    @param paddle pointer to paddle struct */
void paddle_move_left(Paddle* paddle)
{
    // Check that the paddle isn't at the far limit yet
    if (paddle->pos > PADDLE_LEFT_LIMIT) {
        paddle->pos--;
    }
}

/** Move the paddle one unit right if possible:
    @param paddle pointer to paddle struct */
void paddle_move_right(Paddle* paddle)
{
    // Check the paddle isn't at the right limit
    if (paddle->pos < PADDLE_RIGHT_LIMIT) {
        paddle->pos++;
    }
}
