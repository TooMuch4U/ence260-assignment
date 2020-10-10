/** @file communications.h
 * @author Emma Hogan, Tom Rizzi
 * @date 9 October 2020
 * @brief IR communications module
 * last edited 11 October 2020 by Emma Hogan
 */


#ifndef COMMUNICATIONS_H
#define COMMUNICATIONS_H

#include "coder.h"
#include "ir_uart.h"
#include "ball.h"

#define COORD_OFFSET 1
#define DIR_OFFSET 2
#define DEAD_BALL 15 //transmission value for when ball has died


/** transmit relevant ball information:
    @param ball struct containing ball data*/
void transmit_ball (Ball* ball);


/** inform other microcontroller that game has been started
    @param mode 1 for start game, 2 for start round*/
void inform_start (uint8_t mode);


/** Receive relevant ball information from other device
    @param ball struct containing ball data*/
void receive_ball (Ball* ball);


#endif
