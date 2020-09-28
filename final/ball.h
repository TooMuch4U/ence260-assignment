//
// Created by Emma Hogan on 26/09/20.
// Last edited by Emma Hogan on 26/09/20
//

//TODO: Docstring tihs

#ifndef BALL_H
#include "system.h"
#define BALL_H

#define LEFT_WALL 0
#define RIGHT_WALL 6
#define GROUND 0

//TODO : check if signed stuff is ok
#define LEFT -1
#define STRAIGHT 0
#define RIGHT 1
#define UP 1
#define DOWN -1

#define HEIGHT 5


typedef struct ball_s Ball;

struct ball_s {
    //TODO : need uint8?
    int x;
    int y;
    int direction_x; //-1 for left, 0 for straight, 1 for right
    int direction_y; //1 for up, -1 for down
};


void update_location(Ball* ball, int paddle);

void update_x(Ball* ball, int hit_paddle, int paddle);

void update_y (Ball* ball, int hit_paddle);

int has_hit_paddle (Ball* ball, int paddle);

void get_bitmap(uint8_t bitmap[], Ball ball);

int has_hit_ground(Ball* ball);

int is_game_over(Ball* ball);

#endif
