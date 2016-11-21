// ===============================================
// @file   pong.cpp
// @author scollins
// @practical pong
// @brief  Main code for PSP+GLFW(OpenGL) pong game 
// ===============================================

// ===============================================
// Headers
// ===============================================
#include "pong.h"

// ===============================================
// Global identifiers
// ===============================================
float paddle_length;
float paddle_x_pos;
float paddle_y_pos;
float paddle_speed;
float paddle_y_min, paddle_y_max;

// calculate limits for ball movement 
const float ball_y_min = BALL_SIZE/2 + MARGIN_SIZE + BORDER_SIZE;
const float ball_y_max = WINDOW_HEIGHT - BALL_SIZE/2 - BORDER_SIZE;
const float ball_x_min = BORDER_SIZE + BALL_SIZE/2;
const float ball_x_max = WINDOW_WIDTH - PADDLE_WIDTH - BALL_SIZE/2;

bool need_to_seed = true;

Ball balls[BALL_MAX_COUNT];
GameObject powerups[MAX_GAMEOBJECT_COUNT/2];
GameObject traps[MAX_GAMEOBJECT_COUNT/2];

int powerup_count = 0;
int trap_count = 0;
int ball_count = 1;

int game_quit = 0;					// flag - true for user want to quit game
int score = 0;						// number bounces off paddle
int lives = 10;					// number of balls left to play						
int auto_mode = 0;					// flag - true for computer playing

// frame information
double this_time, old_time, dt, start_time;

void start_life() {
	create_ball(ball_count-1);
	
	// initial paddle position
	paddle_x_pos    = WINDOW_WIDTH - PADDLE_WIDTH/2;
	paddle_y_pos    = WINDOW_HEIGHT/2;
	paddle_length   = PADDLE_MAX_LENGTH;
}

// Get random numbers in a range
float random_number(float min, float max) {
	if (need_to_seed) {
		need_to_seed = false;
		srand((int) glfwGetTime());
	}
    float rng = (float)rand() / (float)RAND_MAX;
    return min + rng * (max - min);
}

// creat a new powerup with random x and y values and random type
void create_powerup(int index) {
	powerups[index].x      = random_number(MARGIN_SIZE*2, WINDOW_WIDTH - MARGIN_SIZE*5);
	powerups[index].y      = random_number(BORDER_SIZE*2, WINDOW_HEIGHT - BORDER_SIZE*3);
	powerups[index].type = (int) random_number(1, 3);
}

// creat a new trap with random x and y values and random type
void create_trap(int index) {
	traps[index].x      = random_number(MARGIN_SIZE*2, WINDOW_WIDTH - MARGIN_SIZE*5);
	traps[index].y      = random_number(BORDER_SIZE*2, WINDOW_HEIGHT - BORDER_SIZE*3);
	traps[index].type = (int) random_number(3, 6);
}

// when a new ball is in play set its position to center screen
void create_ball(int index) {
	balls[index].x      = WINDOW_WIDTH/2;
	balls[index].y      = WINDOW_HEIGHT/2;
	balls[index].angle  = random_number(-1.0, 1.0);
}

// Function to remove a ball from play
void kill_ball(int index) {
	balls[index] = balls[ball_count-1];
	ball_count--;
	if (ball_count == 0) {
		ball_count++;
		start_life();
	}
}

// Function to remove a powerup from play
void kill_powerup(int index) {
	powerups[index] = powerups[powerup_count-1];
	powerup_count--;
}

// Function to remove a trap from play
void kill_trap(int index) {
	traps[index] = traps[trap_count-1];
	trap_count--;
}

// check if a one object ovelaps another using contradiction, check all four sides of a rectangle
// example: check if the x of rect1 is > than the x+width of rect2 to check if rect1 is to the right of rect2
// do the same for the otehr 3 sides, if all 4 are true, no collision
bool didCollide(float x1, float y1, float size1, float x2, float y2, float size2) {
	if ( (x1 > (x2 + size2) ) || ((x1 + size1) < x2 ) 
		|| (y1 > (y2 + size2) ) || ((y1 + size1) < y2) ) 
			return false;
	else
		return true;												
}

// Game logic for when a powerup is hit by a ball
void powerup_logic(int type) {
	if (type == 1) {
		if (ball_count < BALL_MAX_COUNT) {
			ball_count++;
			create_ball(ball_count-1);
		}
	} else 
		if (paddle_length < PADDLE_MAX_LENGTH) { 
			paddle_length = PADDLE_MAX_LENGTH;
			if (lives < 10) lives++;
	}
}

// game logic for when a ball hits a trap
void trap_logic(int type, int index) {
	if (type == 3) kill_ball(index);
	else if (type == 4) balls[index].angle = M_PI - balls[index].angle;
	else {
		balls[index].x = random_number(MARGIN_SIZE*2, WINDOW_WIDTH - MARGIN_SIZE*5);
		balls[index].y = random_number(BORDER_SIZE*2, WINDOW_HEIGHT - BORDER_SIZE*3);
	}
}


void spawn_powerups(void) {
	if (powerup_count < MAX_GAMEOBJECT_COUNT/2) {
		int rng = (int) random_number(0, 1000000);
		if (rng < 590700 && rng > 590600) {
			powerup_count++;
			create_powerup(powerup_count-1);
		}
	}
}

void spawn_traps(void) {
	if (trap_count < MAX_GAMEOBJECT_COUNT/2) {
		int rng = (int) random_number(0, 1000000);
		if (rng < 890800 && rng > 890600) {
			trap_count++;
			create_trap(trap_count-1);
		}
	}
}

void spawn_ball(void) {
	if (ball_count < BALL_MAX_COUNT) {
		ball_count++;
		score++;
		create_ball(ball_count - 1);
		paddle_length = PADDLE_MAX_LENGTH;
	}
}

void check_wall_collision(int index) {
	// check - ball hit top or bottom wall
	if (balls[index].y <= ball_y_min) {
		balls[index].angle = -balls[index].angle;
		balls[index].y = ball_y_min + 2; 
	} else if (balls[index].y >= ball_y_max) {
		balls[index].angle = -balls[index].angle;
		balls[index].y = ball_y_max - 2;
	}

	// check - ball hit left wall
	if  (balls[index].x <= ball_x_min) {
		balls[index].angle = M_PI - balls[index].angle;
		balls[index].x = ball_x_min + 2;
	}
}

void check_paddle_hit(int index) {
	if (balls[index].x >= ball_x_max) {
		if (fabs(balls[index].y-paddle_y_pos) <= (paddle_length+BALL_SIZE)/2) {
			balls[index].x = ball_x_max - 2;
			balls[index].angle = M_PI - balls[index].angle;
			score++;
			if (paddle_length > PADDLE_MIN_LENGTH) paddle_length -= 5.0;
		} else {
			if (ball_count == 1)lives--;
			kill_ball(index);
		}
	 }
}

void check_gameobject_collision(int index) {
	// check ball hit powerup
	for (int k = 0; k < powerup_count; ++k) {
		if (didCollide(balls[index].x, balls[index].y, BALL_SIZE, powerups[k].x, powerups[k].y, POWERUP_SIZE)) {
			powerup_logic(powerups[k].type);
			kill_powerup(k);
		}
	}
	// check ball hit trap
	for (int k = 0; k < trap_count; ++k) { 
		if (didCollide(balls[index].x, balls[index].y, BALL_SIZE, traps[k].x, traps[k].y, POWERUP_SIZE)) {
			trap_logic(traps[k].type, index);
			kill_trap(k);
		}				
	}
}

void auto_paddle_move(void) {
	if (paddle_y_pos-paddle_length/2>balls[0].y) {
		paddle_speed = -1;
	} else if (paddle_y_pos+paddle_length/2<balls[0].y) {
		paddle_speed = 1;
	}
}

void update() {
	// calculate limits for paddle movement 
	const float paddle_y_min = (paddle_length/2+MARGIN_SIZE+BORDER_SIZE - 15);
	const float paddle_y_max = (WINDOW_HEIGHT-BORDER_SIZE-paddle_length/2);

	// automatically update paddle direction (if required)
	if (auto_mode) auto_paddle_move();

	// update paddle position
	paddle_y_pos += PADDLE_STEP*paddle_speed;

	// ensure paddle stays within limits
	if (paddle_y_pos<paddle_y_min) {
		paddle_y_pos = paddle_y_min;
	} else if (paddle_y_pos>paddle_y_max) {
		paddle_y_pos = paddle_y_max;
	}
	// spawn game objects
	spawn_powerups();
	spawn_traps();

	// spawn new ball and reset paddle size
	if (score % 5 == 0 && score > 0) spawn_ball();

	for (int i = 0; i < ball_count; i++) {	
		// update ball position (centre)
        balls[i].x += BALL_STEP*cos(balls[i].angle);
        balls[i].y += BALL_STEP*sin(balls[i].angle);
		// check - ball hit powerup or trap
		check_gameobject_collision(i);
	    // check - ball hit wall
		check_wall_collision(i);
		// check - ball hits paddle or misses
		check_paddle_hit(i);
	}
}

int main(void) {
	init();							// initial graphics/sound components
	start_life();					// reset game to starting state

	while (!game_quit && lives>0) {
		draw_scene();
		get_input();
		update();	
	}
	deinit();
	return 0;
}
