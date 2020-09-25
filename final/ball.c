#DEFINE LEFT_WALL 0
#DEFINE RIGHT_WALL 6
#DEFINE GROUND 0

//TODO : check if signed stuff is ok
#DEFINE LEFT -1
#DEFINE STRAIGHT 0
#DEFINE RIGHT 1
#DEFINE UP 1
#DEFINE DOWN -1

typedef struct ball_s Ball

struct ball_s {
    //TODO : need uint8?
    int x;
    int y;
    int direction_x; //-1 for left, 0 for straight, 1 for right
    int direction_y; //1 for up, -1 for down
}

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

int has_hit_ground(Ball ball)
{
    return ball.y == GROUND && ball.direction_y == DOWN;
}

int is_game_over(Ball ball)
{
    return has_hit_ground(ball);
}

