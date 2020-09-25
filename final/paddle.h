//
// Created by tcr59 on 26/09/20.
//

#ifndef ASSIGMENT_PADDLE_H
#define ASSIGMENT_PADDLE_H

#include "navswitch.h"

#define PADDLE_WIDTH 3
#define PADDLE_LEFT_LIMIT 1
#define PADDLE_RIGHT_LIMIT 4
#define PADDLE_START_POS 2

// Paddle instance
Paddle PADDLE_INSTANCE;

void paddle_init();
void paddle_move_check();
void paddle_move_left();
void paddle_move_right();

#endif //ASSIGMENT_PADDLE_H
