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

uint8_t get_paddle_location();
void paddle_init();
void paddle_move_check();
void paddle_move_left();
void paddle_move_right();
void get_paddle_bitmap(uint8_t* bitmap);

#endif //ASSIGMENT_PADDLE_H
