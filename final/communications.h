//
// Created by Emma Hogan on 08/10/20.
// Last edited by Emma Hogan on 08/10/20.
//


#ifndef COMMUNICATIONS_H
#define COMMUNICATIONS_H

#include "coder.h"
#include "ir_uart.h"
#include "ball.h"

#define COORD_OFFSET 1
#define DIR_OFFSET 2
#define DEAD_BALL 15 //transmission value for when ball has died


/** transmit relevant ball information */
void transmit_ball (Ball* ball);

/** inform other microcontroller that game has been started */
void inform_start (uint8_t mode);

/** Receive relevant ball information from other device */
void receive_ball (Ball* ball);


#endif
