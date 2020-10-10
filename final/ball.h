/** @file ball.h
 * @author Emma Hogan, Tom Rizzi
 * @date 26 September 2020
 * @brief ball behaviour module
 * last edited 9 October 2020 by Emma Hogan
 */


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
#define DEAD 1
#define ALIVE 0
#define ON_SCREEN 1
#define OFF_SCREEN 0
#define PADDLE_COL 4
#define BLANK 0x00


/**define data associated with ball */
typedef struct ball_s Ball;


/** Ball structure*/
struct ball_s {
    uint8_t x;
    uint8_t y;
    int8_t direction_x; //-1 for left, 0 for straight, 1 for right
    int8_t direction_y; //1 for up, -1 for down
    uint8_t on_screen; //1 if on screen, 0 if on opponent's screen
    uint8_t dead; //1 if game lost, 0 if still in play
};


/** Initialise ball structure:
    @param ball pointer to struct being initialised 
    @param x x coordinate to initialise ball at
    @param y y coordinate to initialise ball at
    @param x_dir initial horizontal component of direction
    @param y_dir initial vertical component of direction
    @param on_screen 1 if ball on_screen, 0 if on opponents screen*/
void ball_init (Ball* ball, uint8_t x, uint8_t y, int8_t x_dir, int8_t y_dir, uint8_t on_screen);


/** Update location of ball:
    @param ball pointer to struct being initialised 
    @param paddle x coordinate of centre of paddle*/
void update_location (Ball* ball, uint8_t paddle);


/** Return bitmap array representing both ball and paddle:
    @param bitmap current bitmap array to update with new ball location
    @param ball pointer to ball struct */
void get_bitmap (uint8_t bitmap[], Ball* ball);


#endif
