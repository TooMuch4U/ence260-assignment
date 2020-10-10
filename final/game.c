/** @file game.c
 * @author Emma Hogan, Tom Rizzi
 * @date 26 September 2020
 * @brief main module
 * last edited 10 October 2020 by Emma Hogan
 */


#include "system.h"
#include "pacer.h"
#include "ball.h"
#include "coder.h"
#include "paddle.h"
#include "navswitch.h"
#include "ir_uart.h"
#include "pong_display.h"
#include "communications.h"

#define HEIGHT 5
#define BALL_RATE 100
#define DISPLAY_RATE 250
#define DISPLAY_CYCLES 10
#define MESSAGE_RATE 10
#define WINNING_SCORE '5'
#define START_MENU 0
#define PADDLE_MODE 1
#define PLAY_MODE 2
#define DISPLAY_SCORE_MODE 3
#define GAME_OVER_MODE 4
#define INITIAL_COUNTER_VALUE 0
#define INITIAL_SCORE '0'
#define GAME_START_EVENT 1
#define BALL_FIRED_EVENT 2
#define STARTING_MODE 2
#define RECEIVING_MODE 1


typedef struct {
    char score;
    char opponent_score;
    uint8_t game_mode; //game modes: START_MENU = 0, PADDLE_MODE = 1, PLAY_MODE = 2, DISPLAY_SCORE_MODE = 3, GAME_OVER_MODE = 4
    uint8_t ball_counter;
    uint8_t column_counter;
    uint8_t display_counter;
    uint8_t display_cycle; //to count number of passed clock cycles
} Game;


/** Display scrolling PONG text and wait for user to signal they wish to begin game:
    @param game a pointer to the game object */
static void run_start_menu (Game* game)
{
    // scroll the start of game text until one player starts paddle screen
    tinygl_update();
    // Check for navswitch presses
    navswitch_update();

    // Check for a push
    if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
        inform_start(GAME_START_EVENT); //tell other controller a game has been started
        game->game_mode = PADDLE_MODE;
    }
    // Check if the other fun kit pressed start
    if (ir_uart_read_ready_p()) {
        uint8_t val = ir_uart_getc();
        uint8_t decoded_val = decode(val);
        if (decoded_val == GAME_START_EVENT) { //we are receiving a transmission, not noise
            game->game_mode = PADDLE_MODE;
        }
    }
}


/** Interpret navswitch input to update paddle location:
    @param paddle a pointer to the paddle object */
static void move_paddle (Paddle* paddle)
{
    // Check for navswitch presses
    navswitch_update();

    //Check for a left push
    if (navswitch_push_event_p(NAVSWITCH_SOUTH)) {
        paddle_move_left(paddle);
    }
    // Check for a right push
    if (navswitch_push_event_p(NAVSWITCH_NORTH)) {
        paddle_move_right(paddle);
    }
}


/** Initialise the attributes of the ball struct based on position of paddle on launch:
    @param ball a pointer to the ball object
    @param paddle a pointer to the paddle object
    @param game state, an int indicating whether current player or opponent is starting */
static void initialise_ball (Ball* ball, Paddle* paddle, uint8_t game_state)
{
    if (game_state == RECEIVING_MODE) {
        //other player is starting. Initialise ball to be offscreen and wait for transmission
        ball_init(ball, 0, 5, 0, 0, OFF_SCREEN);
    } else {
        //we just started the game. Shoot a ball
        uint8_t paddle_loc = get_paddle_location(paddle);
        ball_init(ball, paddle_loc, 1, 0, UP, ON_SCREEN);
    }
}


/** Run game with paddle movement only and wait for a player to launch a ball and start a round:
    @param ball a pointer to the ball object
    @param paddle a pointer to the paddle object
    @param game a pointer to the game object
    @param bitmap, an array indicating the current ledmat display */
static void run_paddle_only (Ball* ball, Paddle* paddle, Game* game, uint8_t bitmap[])
{
    //run game with paddle only until one user fires a ball

    //listen for move paddle instructions
    move_paddle(paddle);
    get_paddle_bitmap(paddle, bitmap);
    game->column_counter = update_display(bitmap, game->column_counter);

    // Check for a push
    if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
        // release the kraken
        inform_start(BALL_FIRED_EVENT); //tell other controller a ball has been released
        game->game_mode = PLAY_MODE;
        initialise_ball(ball, paddle, STARTING_MODE);
    }

     // Check if the other fun kit pressed start
    if (ir_uart_read_ready_p()) {
        uint8_t val = ir_uart_getc();
        uint8_t decoded_val = decode(val);
        if (decoded_val == BALL_FIRED_EVENT) { //we are receiving a transmission, not noise
            game->game_mode = PLAY_MODE;
            initialise_ball(ball, paddle, RECEIVING_MODE);
        }
    }
}


/** Run the game logic during a round - ball and paddle movement, waiting for a game loss event:
    @param paddle a pointer to the paddle object
    @param ball a pointer to the ball object
    @param game a pointer to the game object
    @param bitmap, an array indicating the current ledmat display */
static void play_round (Paddle* paddle, Ball* ball, Game* game, uint8_t bitmap[])
{
    move_paddle(paddle);
    get_paddle_bitmap(paddle, bitmap);
    get_bitmap(bitmap, ball);
    game->column_counter = update_display(bitmap, game->column_counter);
    game->ball_counter++;

    //if the ball is on screen and the timer is right, update location
    if (ball->on_screen && game->ball_counter > BALL_RATE) {
        game->ball_counter = 0;
        update_location(ball, get_paddle_location(paddle));
        if (!ball->on_screen) {
            //if ball just moved off screen, transmit relevant info
            transmit_ball(ball);
        } else if (ball->dead) {
            //just lost the round
            transmit_ball(ball);
            game->opponent_score++;
            game->game_mode = DISPLAY_SCORE_MODE;
            tinygl_clear(); //clear previous score to prevent delay
        }
    } else if (!ball->on_screen) {
        //listen constantly for transmissions while the ball is offscreen
        receive_ball(ball);
        if (ball->dead) {
            game->score++;
            game->game_mode = DISPLAY_SCORE_MODE;
            tinygl_clear(); //clear previous score to prevent delay
        } else if (ball->on_screen) {
            //reset ball timer
            game->ball_counter = 0;
        }
    }
}


/** Display the updated score for some amount of time and then update game state to
    keep playing or move to a win/loss screen if relevant:
    @param game a pointer to the game object */
static void check_display_timeout(Game* game)
{
    if (game->display_counter > DISPLAY_RATE) {
        game->display_cycle++;
    }
    if (game->display_cycle > DISPLAY_CYCLES) {
        game->display_counter = 0;
        game->display_cycle = 0;
        if (game->score == WINNING_SCORE) {
            game->game_mode = GAME_OVER_MODE;
            scroll_text("WINNER :) ");
        } else if (game->opponent_score == WINNING_SCORE) {
            game->game_mode = GAME_OVER_MODE;
            scroll_text("LOSER :( ");
        } else {
            game->game_mode = PADDLE_MODE;
        }
    }
}


/** Initialise each of the components needed for gameplay */
static void initialise(void)
{
    system_init ();
    pacer_init(PACER_RATE);
    navswitch_init();
    ir_uart_init();
    init_led_matrix();
}


int main (void)
{
    initialise();

    uint8_t bitmap[HEIGHT] = {0};
    Paddle paddle;
    paddle_init(&paddle);
    Ball ball;
    Game game = {INITIAL_SCORE, INITIAL_SCORE, START_MENU, INITIAL_COUNTER_VALUE, INITIAL_COUNTER_VALUE, INITIAL_COUNTER_VALUE, INITIAL_COUNTER_VALUE};
    scroll_text("PONG ");

    while (1) {
        pacer_wait();
        switch(game.game_mode) {
            case START_MENU :
                run_start_menu(&game);
                break;

            case PADDLE_MODE :
                tinygl_clear();
                run_paddle_only(&ball, &paddle, &game, bitmap);
                break;

            case PLAY_MODE :
                play_round(&paddle, &ball, &game, bitmap);
                break;

            case DISPLAY_SCORE_MODE :
                game.display_counter++;
                display_character(game.score);
                check_display_timeout(&game);
                break;

            case GAME_OVER_MODE :
                tinygl_update();
                break;
        }
    }
}
