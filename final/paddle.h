/** @file paddle.h
 * @author Emma Hogan, Tom Rizzi
 * @date 26 September 2020
 * @brief paddle behaviour module
 * last edited 10 October 2020 by Emma Hogan
 */

#ifndef ASSIGMENT_PADDLE_H
#define ASSIGMENT_PADDLE_H

#include "navswitch.h"

#define PADDLE_WIDTH 3
#define PADDLE_LEFT_LIMIT 0
#define PADDLE_RIGHT_LIMIT 6
#define PADDLE_START_POS 1

typedef struct {
    uint8_t pos;
} Paddle;

/** Initialise the paddle struct:
    @param paddle pointer to paddle struct */
void paddle_init(Paddle* paddle);

/** Move the paddle one unit left if possible:
    @param paddle pointer to paddle struct */
void paddle_move_left(Paddle* paddle);

/** Move the paddle one unit right if possible:
    @param paddle pointer to paddle struct */
void paddle_move_right(Paddle* paddle);

/** Returns a bitmap of the current paddle position:
    @param paddle pointer to paddle struct 
    @param bitmap bitmap array for ledmat*/
void get_paddle_bitmap(Paddle* paddle, uint8_t* bitmap);

/** Returns the paddles current position:
    @param paddle pointer to paddle struct
    @return int of paddle position */
uint8_t get_paddle_location(Paddle* paddle);

#endif //ASSIGMENT_PADDLE_H
