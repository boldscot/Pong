// ===============================================
// @file   pong.h
// @author scollins
// @practical pong
// @brief  Headfile for PSP+GLFW(OpenGL) pong game
// ===============================================

#ifndef PONG_H
#define PONG_H

// uncomment next line for PSP executable
//#define PSP 1
 
// ===============================================
// Headers
// ===============================================
#ifdef PSP								// PSP specific headers
	#include <psptypes.h>
	#include <kernel.h>
	#include <psperror.h>		
	#include <displaysvc.h>
	#include <ctrlsvc.h>				// controller 
	#include <libgu.h>					// basic graphics
	#include <libgum.h>					// 3D "OpenGLish" graphics 

#else          						// Unix based system specific
	#include <GL/gl.h>          
	#include <GL/glu.h>         
	#include <GL/glut.h>  
	#include <GL/glfw.h>
#endif

#include <stdio.h>      				// for sprintf()
#include <stdlib.h>     				// for atof() 
#include <string.h>     				// for strcpy()
#include <math.h>       				// for cos(), sin()

// ===============================================
// Global identifiers
// ===============================================

// window & scene
#ifdef PSP   							// psp data types (see psptypes.h)
	#define WINDOW_WIDTH    SCEGU_SCR_WIDTH
	#define WINDOW_HEIGHT   SCEGU_SCR_HEIGHT
	#define MARGIN_SIZE      20
	#define BORDER_SIZE      10
#else
	#define WINDOW_NAME     "Pong using GLFW"
	#define WINDOW_WIDTH    640
	#define WINDOW_HEIGHT   480
	#define MARGIN_SIZE      15
	#define BORDER_SIZE      18
#endif

// ball
#define BALL_SIZE        15
#ifdef PSP   					
	#define BALL_STEP    5 			// distance ball travels per time step
#else
	#define BALL_STEP    dt*400  	
#endif

// paddle
#define PADDLE_WIDTH     BORDER_SIZE
#define PADDLE_MAX_LENGTH 200
#define PADDLE_MIN_LENGTH 30
#ifdef PSP   		
	#define PADDLE_STEP  10    			// distance paddle travels per time step
#else
	#define PADDLE_STEP  dt*520
#endif
#define POWERUP_SIZE	20

struct GameObject {
	float x, y;
	int type;
};

const int MAX_GAMEOBJECT_COUNT = 4;
extern GameObject powerups[];
extern GameObject traps[];

struct Ball {
	float x, y, angle;
};

const int BALL_MAX_COUNT = 5;
extern Ball balls[];

extern float paddle_length;				// Length of the paddle
extern float paddle_x_pos;
extern float paddle_y_pos;
extern float paddle_speed;

extern int game_quit;					// flag - true for user want to quit game
extern int auto_mode;					// flag - true for computer playing
extern int score;					// number bounces off paddle
extern int lives;					// number of balls left to play

extern int ball_count;					// number of balls in play	
extern int powerup_count;
extern int trap_count;

extern bool need_to_seed;

// frame information
extern double this_time, old_time, dt, start_time;

// ===============================================
// Function prototypes
// ===============================================

// platform generic functions
void start_life(void);					// restart game for new life
void update(void);						// update game entities

// function with platform specific implementations in pong_*.c
int init(void);							// initialise graphics system
void deinit(void);						// release resources

void draw_rectangle(float x1, float y1, float x2, float y2);
void draw_walls(void);					
void draw_paddle(void); 
int draw_scene(void);					// draw scene by calling draw_* functions
void draw_status(void);					// draw status message

void setColor(float r, float g, float b);

void create_ball(int index);
void draw_ball(void);	
void kill_ball(int index);

bool didCollide(float x1, float y1, float size1, float x2, float y2, float size2);
float random_number(float min, float max);

void create_powerup(int index);
void draw_powerup(void);
void draw_trap(void);
void kill_powerup(int index);
void kill_trap(int index);

void powerup_logic(int type);
void trap_logic(int type);
		
void get_input(void);					// keyboard/mouse/pad input

#endif
