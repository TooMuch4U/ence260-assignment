//
// Created by Emma Hogan on 26/09/20.
// Last edited by Emma Hogan on 26/09/20
//

#include "ball.h"

void ball_init (Ball* ball, uint8_t x, uint8_t y, int8_t x_dir, int8_t y_dir, uint8_t on_screen)
{
    ball->x = x;
    ball->y = y;
    ball->direction_x = x_dir;
    ball->direction_y = y_dir;
    ball->on_screen = on_screen;
}

void update_location(Ball* ball, uint8_t paddle)
{
    uint8_t hit_paddle = has_hit_paddle(ball, paddle);
    update_x(ball, hit_paddle, paddle);
    update_y(ball, hit_paddle);
    if (has_gone_off_screen(ball)) {
        ball->on_screen = 0;
    }
}

void update_x(Ball* ball, uint8_t hit_paddle, uint8_t paddle)
{
    if (ball->x == LEFT_WALL && ball->direction_x == LEFT) {
        //hit the left wall
        ball->direction_x = RIGHT;
        ball->x++;
    } else if (ball->x == RIGHT_WALL && ball->direction_x == RIGHT) {
        //hit the right wall
        ball->direction_x = LEFT;
        ball->x--;
    } else if (hit_paddle) {
        ball->direction_x = 0;
        if (ball->x == paddle - 1) {
            //hit left side of paddle - bounce off left
            ball->x--;
            ball->direction_x = LEFT;
        } else if (ball->x == paddle) {
            //no change to ball direction
            ball->x += ball->direction_x;
        } else {
            //ball hit right side of paddle - bounce off right
            ball->x++;
            ball->direction_x = RIGHT;
        }
    } else {
        //continue moving in same direction
        ball->x += ball->direction_x;
    }
}

void update_y (Ball* ball, uint8_t hit_paddle)
{
    if (has_hit_ground(ball)) {
        //lost the game
        //for now
        ball->direction_y = UP;
    } else if (hit_paddle) {
        //ball hit the paddle
        ball->y++;
        ball->direction_y = UP;
    } else {
        //continue in same direction
        ball->y += ball->direction_y;
    }
}

uint8_t has_hit_paddle (Ball* ball, uint8_t paddle)
{
    if (ball->y == GROUND + 1 && ball->direction_y == DOWN) {
        if (ball->x == paddle || ball->x - 1 == paddle || ball->x + 1 == paddle) {
            return 1;
        }
    }
    return 0;
}

uint8_t has_gone_off_screen (Ball* ball)
{
    return ball->y >= HEIGHT;
}

void get_bitmap(uint8_t bitmap[], Ball ball)
{
    for (uint8_t i = 0; i < HEIGHT; i++) {
        if (i != (HEIGHT - 1 - ball.y)) {
            bitmap[i] = 0x00;
        } else {
            bitmap[i] = 1 << (RIGHT_WALL - ball.x);
        }
    }
}

uint8_t has_hit_ground(Ball* ball)
{
    return ball->y == GROUND && ball->direction_y == DOWN;
}

uint8_t is_game_over(Ball* ball)
{
    return has_hit_ground(ball);
}


