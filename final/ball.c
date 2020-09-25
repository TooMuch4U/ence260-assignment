//
// Created by Emma Hogan on 26/09/20.
// Last edited by Emma Hogan on 26/09/20
//

#include "ball.h"

void update_location(Ball ball, int paddle)
{
    update_x(ball);
    ball.y += ball.direction_y;
}

void update_x(Ball ball)
{
    if (ball.x == LEFT_WALL && ball.direction_x == LEFT) {
        //hit the left wall
        ball.direction_x = RIGHT;
        ball.x++;
    } else if (ball.x == RIGHT_WALL && ball.direction_x == RIGHT) {
        //hit the right wall
        ball.direction_x = LEFT;
        ball.x--;
    } else if (has_hit_paddle (ball, paddle) {
        if (ball.x == paddle - 1 && ball.direction_x != LEFT) {
            //hit left side of paddle - change direction to be more lefty
            ball.x--;
            ball.direction_x--;
        } else if (ball.x == paddle + 1 && ball.direction_x != RIGHT) {
            //hit right side of paddle - change direction to be more righty
            ball.x++;
            ball.direction_x++;
        } else {
            //hit paddle but no need to change direction
            ball.x += ball.direction_x;
        }
    } else {
        //continue moving in same direction
        ball.x += ball.direction_x;
    }
}

void update_y (Ball ball, int paddle)
{
    if (has_hit_ground(ball)) {
        //lost the game
    } else if (has_hit_paddle(ball, paddle)) {
        //ball hit the paddle
        ball.y++;
        ball.direction_y = UP;
        ball.direction_x *= -1;
    } else {
        //continue in same direction
        ball.y += direction_y;
    }
}

int has_hit_paddle (Ball ball, int paddle)
{
    if (ball.y == GROUND + 1 && ball.direction_y == DOWN) {
        if (ball.x == paddle || ball.x - 1 == paddle || ball.x + 1 == paddle) {
            return 1;
        }
    }
    return 0;
}

void get_bitmap(uint8_t* bitmap, size_t len, Ball ball)
{
    for (int i = 0; i < len; i++) {
        if (i != ball.y) {
            bitmap[i] = 0x00;
        } else {
            bitmap[i] = 1 << (RIGHT_WALL - ball.x);
        }
    }
}

int has_hit_ground(Ball ball)
{
    return ball.y == GROUND && ball.direction_y == DOWN;
}

int is_game_over(Ball ball)
{
    return has_hit_ground(ball);
}


