//
// Created by Emma Hogan on 26/09/20.
// Last edited by Emma Hogan on 05/10/20
//

//TODO: Docstring tihs

#ifndef BALL_H
#define BALL_H

#include "system.h"

#define LEFT_WALL 0
#define RIGHT_WALL 6
#define GROUND 0
#define HEIGHT 5

#define LEFT -1
#define STRAIGHT 0
#define RIGHT 1
#define UP 1
#define DOWN -1




typedef struct ball_s Ball;

struct ball_s {
    uint8_t x;
    uint8_t y;
    int8_t direction_x; //-1 for left, 0 for straight, 1 for right
    int8_t direction_y; //1 for up, -1 for down
    uint8_t on_screen; //1 if on screen, 0 if on opponent's screen
};

void ball_init (Ball* ball, uint8_t x, uint8_t y, int8_t x_dir, int8_t y_dir, uint8_t on_screen);

void update_location(Ball* ball, uint8_t paddle);

void update_x(Ball* ball, uint8_t hit_paddle, uint8_t paddle);

void update_y (Ball* ball, uint8_t hit_paddle);

uint8_t has_hit_paddle (Ball* ball, uint8_t paddle);

void get_bitmap(uint8_t bitmap[], Ball ball);

uint8_t has_gone_off_screen (Ball* ball);

uint8_t has_hit_ground(Ball* ball);

uint8_t is_game_over(Ball* ball);

#endif
