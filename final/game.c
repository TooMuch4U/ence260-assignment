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
#define PACER_RATE 400
#define MESSAGE_RATE 10
#define WINNING_SCORE '5'
#define START_MENU 0
#define PADDLE_MODE 1
#define PLAY_MODE 2
#define DISPLAY_SCORE_MODE 3
#define GAME_OVER_MODE 4


typedef struct {
    char score;
    char opponent_score;
    uint8_t game_mode; //game modes: START_MENU = 0, PADDLE_MODE = 1, PLAY_MODE = 2, DISPLAY_SCORE_MODE = 3, GAME_OVER_MODE = 4
    uint8_t ball_counter;
    uint8_t column_counter;
    uint8_t display_counter;
} Game;


/** display scrolling PONG text and wait for user to signal they wish to begin game */
static void run_start_menu (Game* game)
{
    // scroll the start of game text until one player starts paddle screen
    //scroll_text("PONG ");
    tinygl_update();
    // Check for navswitch presses
    navswitch_update();

    // Check for a push
    if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
        inform_start(1); //tell other controller a game has been started
        game->game_mode = PADDLE_MODE;
    }
    // Check if the other fun kit pressed start
    if (ir_uart_read_ready_p()) {
        uint8_t val = ir_uart_getc();
        uint8_t decoded_val = decode(val);
        if (decoded_val == 1) { //we are receiving a transmission, not noise
            game->game_mode = PADDLE_MODE;
        }
    }
}

/** interpret navswitch input to update paddle location */
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


/** initialise the attributes of the ball struct based on position of paddle on launch */
static void initialise_ball (Ball* ball, Paddle* paddle, uint8_t game_state)
{
    if (game_state == 1) {
        //other player is starting. Initialise ball to be offscreen and wait for transmission
        ball_init(ball, 0, 5, 0, 0, 0);
    } else {
        //we just started the game. Shoot a ball
        uint8_t paddle_loc = get_paddle_location(paddle);
        ball_init(ball, paddle_loc, 1, 0, 1, 1);
    }
}



/** run game with paddle movement only and wait for a player to launch a ball and start a round */
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
        inform_start(2); //tell other controller a ball has been released
        game->game_mode = PLAY_MODE;
        initialise_ball(ball, paddle, 2);
    }

     // Check if the other fun kit pressed start
    if (ir_uart_read_ready_p()) {
        uint8_t val = ir_uart_getc();
        uint8_t decoded_val = decode(val);
        if (decoded_val == 2) { //we are receiving a transmission, not noise
            game->game_mode = PLAY_MODE;
            initialise_ball(ball, paddle, 1);
        }
    }
}

/** for debugging purposes */
static void blue_led (void)
{
    //freezes program, use for debugging
    /* Initialise port to drive LED 1.  */
    DDRC |= (1 << 2);

    while (1)
    {
        /* Set port to turn LED 1 on.  */
        PORTC |= (1 << 2);
    }
}

static void play_round (Paddle* paddle, Ball* ball, Game* game, uint8_t bitmap[])
{
    move_paddle(paddle);
    get_paddle_bitmap(paddle, bitmap);
    get_bitmap(bitmap, ball);
    game->column_counter = update_display(bitmap, game->column_counter);

    game->ball_counter++;

    //if the ball is on screen and the timer is right, update location
    if (ball->on_screen) {
        if (game->ball_counter > 100) {
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
            }
        }
    } else {
        //listen constantly for transmissions while the ball is offscreen
        receive_ball(ball);
        if (ball->dead) {
            game->score++;
            game->game_mode = DISPLAY_SCORE_MODE;
        } else if (ball->on_screen) {
            //reset ball timer
            game->ball_counter = 0;
        }
    }
}

static void check_display_timeout(Game* game)
{
    if (game->display_counter > 250) {
        game->display_counter = 0;
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

    uint8_t bitmap[5] = {0};
    Paddle paddle;
    paddle_init(&paddle);
    Ball ball;
    Game game = {'0', '0', 0, 0, 0, 0};
    scroll_text("PONG ");

    while (1) {
        pacer_wait();
        switch(game.game_mode) {
            case START_MENU:
                run_start_menu(&game);
                break;

            case PADDLE_MODE :
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
