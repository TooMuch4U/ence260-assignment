/** @file ball.c
 * @author Emma Hogan, Tom Rizzi
 * @date 26 September 2020
 * @brief ball behaviour module
 * last edited 9 October 2020 by Emma Hogan
 */

#include "ball.h"

/** Return true if ball has hit the paddle:
    @param ball pointer to ball struct 
    @param paddle x coordinate of centre of paddle
    @return 1 if hit, else 0*/
static uint8_t has_hit_paddle (Ball* ball, uint8_t paddle)
{
    if (ball->y == GROUND + 1 && ball->direction_y == DOWN) {
        if (ball->x == paddle || ball->x - 1 == paddle || ball->x + 1 == paddle) {
            return 1;
        }
    }
    return 0;
}

/** Return true if ball has flown off screen:
    @param ball pointer to ball struct 
    @return 1 if ball is offscreen else 0*/
static uint8_t has_gone_off_screen (Ball* ball)
{
    return ball->y >= HEIGHT;
}

/** Return true if ball has hit the ground:
    @param ball pointer to ball struct 
    @return 1 if ball is on ground else 0*/
static uint8_t has_hit_ground(Ball* ball)
{
    return ball->y == GROUND && ball->direction_y == DOWN;
}

/** Update x component of balls location:
    @param ball pointer to ball struct 
    @param hit_paddle 1 if ball has hit paddle, otherwise 0
    @param paddle x coordinate of centre of paddle*/
static void update_x(Ball* ball, uint8_t hit_paddle, uint8_t paddle)
{
    if (ball->x == LEFT_WALL && ball->direction_x == LEFT) {
        //hit the left wall
        ball->direction_x = RIGHT;
        ball->x++;
    } else if (ball->x == RIGHT_WALL && ball->direction_x == RIGHT) {
        //hit the right wall
        ball->direction_x = LEFT;
        ball->x--;
    }else if (hit_paddle) {
        //check that paddle isn't colliding with wall as well as updating direction
        ball->direction_x = 0;
        if (ball->x == paddle - 1 && ball->x != LEFT_WALL) {
            //hit left side of paddle - bounce off left
            ball->x--;
            ball->direction_x = LEFT;
        } else if (ball->x == paddle) {
            //no change to ball direction
            ball->x += ball->direction_x;
        } else if (ball->x != RIGHT_WALL) {
            //ball hit right side of paddle - bounce off right
            ball->x++;
            ball->direction_x = RIGHT;
        }
    } else {
        //continue moving in same direction
        ball->x += ball->direction_x;
    }
}

/** Update y component of balls location:
    @param ball pointer to ball struct 
    @param hit_paddle 1 if ball has hit paddle, otherwise 0*/
static void update_y (Ball* ball, uint8_t hit_paddle)
{
    if (has_hit_ground(ball)) {
        ball->dead = 1;
    } else if (hit_paddle) {
        //ball hit the paddle
        ball->y++;
        ball->direction_y = UP;
    } else {
        //continue in same direction
        ball->y += ball->direction_y;
    }
}

/** Initialise ball structure:
    @param ball pointer to struct being initialised 
    @param x x coordinate to initialise ball at
    @param y y coordinate to initialise ball at
    @param x_dir initial horizontal component of direction
    @param y_dir initial vertical component of direction
    @param on_screen 1 if ball on_screen, 0 if on opponents screen*/
void ball_init (Ball* ball, uint8_t x, uint8_t y, int8_t x_dir, int8_t y_dir, uint8_t on_screen)
{
    ball->x = x;
    ball->y = y;
    ball->direction_x = x_dir;
    ball->direction_y = y_dir;
    ball->on_screen = on_screen;
    ball->dead = 0; //set ball to not be dead initially
}

/** Update location of ball:
    @param ball pointer to ball struct 
    @param paddle x coordinate of centre of paddle*/
void update_location(Ball* ball, uint8_t paddle)
{
    uint8_t hit_paddle = has_hit_paddle(ball, paddle);
    update_x(ball, hit_paddle, paddle);
    update_y(ball, hit_paddle);
    if (has_gone_off_screen(ball)) {
        ball->on_screen = 0;
    }
}

/** Return bitmap array representing both ball and paddle:
    @param bitmap current bitmap array to update with new ball location
    @param ball pointer to ball struct */
void get_bitmap(uint8_t bitmap[], Ball* ball)
{
    uint8_t paddle = bitmap[4];
    for (uint8_t i = 0; i < HEIGHT; i++) {
        if (i != (HEIGHT - 1 - ball->y)) {
            bitmap[i] = 0x00;
        } else {
            bitmap[i] = 1 << (RIGHT_WALL - ball->x);
        }
    }
    bitmap[4] |= paddle; //keep paddle bits
}


