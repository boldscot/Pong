// ===============================================
// @file   pong_glfw.cpp
// @author scollins
// @practical pong
// @brief  GLFW (OpenGL) specific code for Pong implementation
// ===============================================

// ===============================================
// Headers
// ===============================================
#include "pong.h"

#ifndef PSP

// helper function taken from GLUI library
// used to render a string 
void _glutBitmapString(void *font, char *s) {
    char *p = s;

    while(*p != '\0') {
        glutBitmapCharacter(font, *p);
        p++;
    }
}


int init(void) {

	static int argc=1;
	static char *argv[1];
	argv[0] = strdup("pong");
	glutInit(&argc, argv);
	
	glfwInit(); 
	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
	if( !glfwOpenWindow(WINDOW_WIDTH,WINDOW_HEIGHT, 0,0,0,0,0,0, GLFW_WINDOW) ) { 
		glfwTerminate(); 
		return 1; 
	} 
	glfwSetWindowPos(100,100);
	glfwSetWindowTitle(WINDOW_NAME);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);
	
	// frame information
    start_time = this_time = glfwGetTime();

	return 0;
}


void deinit(void) {
	glfwTerminate(); 
}


void get_input(void) {

	game_quit = glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED);
	if (game_quit) return;

	if (glfwGetKey(GLFW_KEY_UP)) {
        	paddle_speed = 1;
    	} else if (glfwGetKey(GLFW_KEY_DOWN)) {
        	paddle_speed = -1;
    	} else {
        	paddle_speed = 0.0;
    	}		
	if (glfwGetKey('A')) auto_mode = !auto_mode;
}


int draw_scene(void) {
	
	glClear(GL_COLOR_BUFFER_BIT); 
	
	draw_walls();
	
	// paddle
    	glPushMatrix();
        	glTranslatef(paddle_x_pos-PADDLE_WIDTH/2, paddle_y_pos-paddle_length/2, 0);
        	draw_paddle();
    	glPopMatrix();

	// ball
	for (int i = 0; i < ball_count; i++) {
		glPushMatrix();
        	glTranslatef(balls[i].x - BALL_SIZE/2, balls[i].y  - BALL_SIZE/2, 0);
        	draw_ball();
    	glPopMatrix();
	}

	for (int i = 0; i < powerup_count; i++) {
		if (powerups[i].type == 1) setColor(0, 255, 255);
		else setColor(255, 255, 0);
		glPushMatrix();
			glTranslatef(powerups[i].x, powerups[i].y, 0);
			draw_powerup();
		glPopMatrix();
	}

	for (int j = 0; j < trap_count; j++) {
		if (traps[j].type == 3) setColor(255, 0, 0);
		else if (traps[j].type == 4) setColor(255, 165, 0);
		else setColor(165, 42, 42);
		glPushMatrix();
			glTranslatef(traps[j].x, traps[j].y, 0);
			draw_powerup();
		glPopMatrix();
	}
    	
	draw_status();								// status message

	glfwSwapBuffers(); 

	// update frame timer
	old_time = this_time;
	this_time = glfwGetTime();
	dt = this_time - old_time;
	
	return 0;	// success
}

//Function used to draw all game objects. 
void draw_rectangle(float x1, float y1, float x2, float y2) {
	glBegin(GL_QUADS) ;
		glVertex2f(x1, y1);
		glVertex2f(x2, y1) ;
		glVertex2f(x2, y2) ;
		glVertex2f(x1, y2) ;
	glEnd() ;
}

// Function to change color of all game objects
void setColor(float r, float g, float b) {
	glColor3f(r/255, g/255, b/255);
}

void draw_walls(void) {
    setColor(255, 0, 0);
	// top wall
	draw_rectangle(0, WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT-BORDER_SIZE);
	//left wall
	draw_rectangle(0, MARGIN_SIZE, BORDER_SIZE, WINDOW_HEIGHT-BORDER_SIZE);
	//lower wall
	draw_rectangle(0, 0, WINDOW_WIDTH, BORDER_SIZE);
}

void draw_powerup(void) {
	draw_rectangle(0, 0, POWERUP_SIZE, POWERUP_SIZE);
}

void draw_ball(void)  {
    setColor(0, 255, 0);
	draw_rectangle(0, 0, BALL_SIZE, BALL_SIZE);
}

void draw_paddle(void)  {
	setColor(0, 0, 255);
	draw_rectangle(0, 0, PADDLE_WIDTH, paddle_length);	
}

void draw_status(void)  {
	setColor(255, 255,0);
	// buffer to hold output message
	static char mess[300];

	sprintf(mess, 
		"FPS = %4.0f  Score = %d   Lives = %1d   Balls = %d", 1.0/dt, score, lives, ball_count);

	glRasterPos2i(100, 1);
    	_glutBitmapString(GLUT_BITMAP_9_BY_15, mess);
}
#endif
