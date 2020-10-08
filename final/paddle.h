//
// Created by tcr59 on 26/09/20.
//

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

void paddle_init(Paddle* paddle);
void paddle_move_check(Paddle* paddle);
void paddle_move_left(Paddle* paddle);
void paddle_move_right(Paddle* paddle);
void get_paddle_bitmap(Paddle* paddle, uint8_t* bitmap);
uint8_t get_paddle_location(Paddle* paddle);

#endif //ASSIGMENT_PADDLE_H
