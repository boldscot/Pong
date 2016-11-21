// ===============================================
// @file   pong_psp.cpp
// @author kmurphy
// @practical pong
// @brief  PSP specific code for Pong implementation
// ===============================================

// ===============================================
// Headers
// ===============================================
#include "pong.h"

#ifdef PSP

// Every program module must have a name and a module version. 
//   (see page 5 PSP Programming Tutorial)
// 
//   This macro has parameters: name, attribute, major, minor 
//   
//   name specified the name of the module and can be up to 
//        27 characters in length and follows standard C rules for
//		identifiers. The module name is used by the debugger to
//		identify modules.
//  attribute
//        Flag used to permit/stop loading of modules with same name
//		
SCE_MODULE_INFO( pong, 0, 1, 1 );

// allocate memory to store display list - list of graphic primitive to draw
// This is used by the libgu library which is the basic graphics library for the PSP
#define MAX_LIST_SIZE 0x1000
static char				disp_list[MAX_LIST_SIZE] __attribute__((aligned(64)));

// allocate memory to store 2 projection, 2 view, 2 world and 0 texture matrices
// This is used by the libgum library which supports 3D operations.
static ScePspFMatrix4	matrix_stack[2+4+4+0];

// screen size is not changing on the PSP so build court once
static const float walls [] = {
	// top wall
	0.0f, WINDOW_HEIGHT, 0.0f,
	WINDOW_WIDTH,WINDOW_HEIGHT-BORDER_SIZE, 0.0f,
	// left wall
	0.0f, (WINDOW_HEIGHT-BORDER_SIZE), 0.0f,
	BORDER_SIZE,  MARGIN_SIZE+BORDER_SIZE, 0.0f,
  	// lower border
	0.0f, MARGIN_SIZE, 0.0f,
	WINDOW_WIDTH, MARGIN_SIZE+BORDER_SIZE, 0.0f
	};

static const float ball[] = {0,0,0, BALL_SIZE,BALL_SIZE,0};
static const float paddle[] = {0,0,0, PADDLE_WIDTH,PADDLE_LENGTH,0};


int init(void) {

	// initialise the graphics library
	if (SCE_OK != sceGuInit()) {
		printf("Error in initialising graphics library\n");
		return 1;
	}

	// start creating the display list 
	// Flag SCEGU_IMMEDIATE means that the display list sequence are 
	// transfreed to the Graphics Engine and drawn at the 
	// same time as the display is being created.
	sceGuStart(SCEGU_IMMEDIATE, disp_list, sizeof(disp_list));

	// setup buffers for smooth animation
	// The depth buffer stores the depth of each pixel in the frame.
	sceGuDrawBuffer(SCEGU_PF5551, SCEGU_VRAM_BP_0, SCEGU_VRAM_WIDTH);
	sceGuDispBuffer(SCEGU_SCR_WIDTH, SCEGU_SCR_HEIGHT,
		SCEGU_VRAM_BP_1, SCEGU_VRAM_WIDTH);			
	sceGuDepthBuffer(SCEGU_VRAM_BP_2, SCEGU_VRAM_WIDTH);

	// specify offset used when transferring from screen coordinate system
	// to drawing coordinate system
	sceGuOffset(SCEGU_SCR_OFFSETX, SCEGU_SCR_OFFSETY);

	// set the viewport: x and y of  top left corner, width, height
	sceGuViewport(2048, 2048, SCEGU_SCR_WIDTH, SCEGU_SCR_HEIGHT);

	// set the depth range for display list primitives
	sceGuDepthRange(10000, 50000);

	// speciffy clipping region 
	// and enable scissor test when drawing primitives
	sceGuScissor(0, 0, SCEGU_SCR_WIDTH, SCEGU_SCR_HEIGHT);
	sceGuEnable(SCEGU_SCISSOR_TEST);

	// set color: red,green,blue,alpha
	sceGuColor(0x00ff00ff);

	// Initialise matrix stack by allocating separate stack space for 
	// projection, view, world and texture matrices.
	sceGumSetMatrixStack(matrix_stack, 2, 4, 4, 0);

	// Set the matrix mode. Can be projection, view, world, or texture 
	sceGumMatrixMode(SCEGU_MATRIX_PROJECTION);

	// Load the identity matrix - multplying the identity leaves vectors unchanged
	sceGumLoadIdentity();

	// Create a orthographic (2D) or perspective view (3D) 
	// For perspective view (3D) specify 
	//    camera angle of view = 45 deg,
	//    screen aspect ratio = SCEGU_SCR_ASPECT,
	//    screen near clipping plane (distance from viewpoint) = 1,
	//    screen far clipping plane (distance from viewpoint) = 1000
	//    eg
	//       sceGumPerspective(SCEGU_RAD(45.0f), SCEGU_SCR_ASPECT, 1.0f, 1000.0f);
	// For orthographic (2D) specify the range in x, y and z 
	//    eg
	//		sceGumOrtho(0, SCEGU_SCR_WIDTH,0, SCEGU_SCR_HEIGHT, -100.0f, 100.0f);
	// In both cases the projection matrix is multipled by the current matrix 
	// (identity) and assigns it the the current matrix.
	sceGumOrtho(0, SCEGU_SCR_WIDTH,0, SCEGU_SCR_HEIGHT, -100.0f, 100.0f);
	
	// change mode to world 
	sceGumMatrixMode(SCEGU_MATRIX_WORLD);

	// end the creation of the display list
	// note - All that is in the display list to date the screen and viewpoint setup
	int size = sceGuFinish();
	if (size>MAX_LIST_SIZE) {
		printf("Error - Insufficient memory alloced for display_list.\n");
		return 1;		
	}

	// Tell drawing prosessor to wait for end of drawing before rendering
	sceGuSync(SCEGU_SYNC_FINISH, SCEGU_SYNC_WAIT);
	
	// Put the drawing thread in wait state until the start of next VBLANK interval
	sceDisplayWaitVblankStart();
	
	// Turn screen disply on
	sceGuDisplay(SCEGU_DISPLAY_ON);

	// Turn on analog input (by default is to save power)
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_DIGITALANALOG);

	return 0;	// success
}

	
void deinit() {
	
	sceGuTerm();
}


void draw_paddle(void) {
		
  	sceGuColor(0xff00ffff);
	sceGumDrawArray(SCEGU_PRIM_RECTANGLES, SCEGU_VERTEX_FLOAT, 2, NULL, paddle);
}


void draw_ball(void) {
	
  	sceGuColor(0xff0000ff);
	sceGumDrawArray(SCEGU_PRIM_RECTANGLES, SCEGU_VERTEX_FLOAT, 2, NULL, ball);
}


void draw_walls(void) {
	
  	sceGuColor(0xffc08040);
	sceGumDrawArray(SCEGU_PRIM_RECTANGLES, SCEGU_VERTEX_FLOAT, 6, NULL, walls);
}


void draw_status(void) {
	
	// buffer to hold output message
	static char mess[100];

	sprintf(mess, "Score = %3d", score);
	sceGuDebugPrint(MARGIN_SIZE,WINDOW_HEIGHT-MARGIN_SIZE/2+2, 0x00ffffff, mess);
}


int draw_scene(void) {
	
	ScePspFVector3 pos;
	sceGuStart(SCEGU_IMMEDIATE, disp_list, sizeof(disp_list));
	sceGuClear(SCEGU_CLEAR_ALL);

	// walls
	draw_walls();

	// ball
	sceGumPushMatrix();
	sceGumLoadIdentity();
	pos.x = ball_x_pos - BALL_SIZE/2;
	pos.y = ball_y_pos - BALL_SIZE/2;
	pos.z = 0;
	sceGumTranslate(&pos);
	draw_ball();
	sceGumPopMatrix();

	// paddle
	sceGumPushMatrix();
	sceGumLoadIdentity();
	pos.x = paddle_x_pos - PADDLE_WIDTH/2;
	pos.y = paddle_y_pos - PADDLE_LENGTH/2;
	sceGumTranslate(&pos);
	draw_paddle();
	sceGumPopMatrix();
	
	// status message
	draw_status();

	int size = sceGuFinish();
	if (size>MAX_LIST_SIZE) {
		printf("Error - Insufficient memory alloced for display_list.\n");
		return 1;		
	}
	
	sceGuSync(SCEGU_SYNC_FINISH, SCEGU_SYNC_WAIT);
	sceDisplayWaitVblankStart();
	sceGuSwapBuffers();
}


void get_input(void) {
	
	static u_int32	prev;					// button status on previous call
	u_int32 		cont;					// current button status
	u_int32		 	padd;					// when buttons have JUST BE PRESSED
 	SceCtrlData		buf;					// store both button and analog status

	sceCtrlReadBufferPositive(&buf, 1);		// populate buf from input queue
	cont = buf.Buttons;						// extract current buttons status 
	padd = cont & (cont ^ prev);			// which buttons has just been pressed?
	prev = cont;							// store button status for next call

	if (padd & SCE_CTRL_SELECT) {			
		game_quit = 1;
		return;
	}

	if (padd & SCE_CTRL_START) {
		score = 0;
		lives = 3;
		start_life();
		return;
	}
	
	if (padd & SCE_CTRL_TRIANGLE) {
		auto_mode = 1 - auto_mode;
		return;
	}
	
	paddle_speed = 0;

	if (cont & SCE_CTRL_CIRCLE) {
		paddle_speed = 1;
		return;
	}	
	if (cont & SCE_CTRL_CROSS) {
		paddle_speed = -1;
		return;
	}
	
	float dy = ( buf.Ly - 128 ) / 128.0f ;
	if ( dy > -0.75f && dy < 0.75f ) dy = 0.0f ;
	paddle_speed = -dy*2; 
}

#endif
