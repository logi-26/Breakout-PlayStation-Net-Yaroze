// Import Net Yaroze libraries
#include <libps.h>
#include "Pad.h"

#include <stdio.h>
#include <string.h>

#include "myassert.h"

#define TRUE 1
#define FALSE 0

#define NONE 32766
#define SIGN_OF(number) (((number) >= 0) ? 1 : -1)
#define OT_LENGTH	1	

#define MAX_SIZEOF_PRIMITIVE 24

#define MAX_BLOCKS 70
#define MAX_OBJECTS (MAX_BLOCKS + 2)	

#define BLOCK_WIDTH 20
#define BLOCK_HEIGHT 10

// Screen area
#define SCREEN_WIDTH 320 
#define SCREEN_HEIGHT 240
#define SCREEN_X_OFFSET 0
#define SCREEN_Y_OFFSET 16

// Arbitrary walls
#define MIN_X 7
#define MAX_X 313
#define MIN_Y 16
#define MAX_Y 235

// Refer to edges and corners of blocks
#define TOP 1
#define BOTTOM 2
#define LEFT 3
#define RIGHT 4
#define TOP_LEFT 5
#define TOP_RIGHT 6
#define BOTTOM_LEFT 7
#define BOTTOM_RIGHT 8
#define MIDDLE 9
#define OVERLAP 10

// Maximum speeds for the bat
#define BAT_SPEED 4
#define MAX_BALL_X_SPEED 5
#define MAX_BALL_Y_SPEED 5

#define KeepWithinRange(x, l, h) ((x)=((x)<(l)?(l):(x)>(h)?(h):(x)))	

// Ordering table related variables
GsOT		WorldOrderingTable[2];
GsOT_TAG	OrderingTableTags[2][1<<OT_LENGTH];
PACKET		GpuPacketArea[2][MAX_OBJECTS * MAX_SIZEOF_PRIMITIVE];

// Storage for controller information
volatile u_char *bb0, *bb1;
static u_long PadRead(void);

// Controller pad buffer
u_long PadStatus;

// Check if user wants to quit
int QuitFlag = FALSE;

enum gameState {titleScreen, playing, ended, paused};

// Prototypes
int main(void);
int GetActiveBuffer(void);
void InitialiseGraphicsSystem(void);
void InitialiseAllElements(void);
void InitialiseController(void);
void InitialiseBackground(void);
void InitialiseBlocks (void);
void InitialiseBat(void);
void InitialiseBall(void);
void InitialiseGameObjects(void);
void SetInitialGameValues();
void UpdateGraphicsSystem(int activeBuffer);
u_long PadRead(void);
void HandleControllerInput(void);
void HandleAllObjects(void);
void MoveBallOutOfBat(void);
void DestroySingleBlock (int);
void Delay (int n);
int RectanglesOverlap(GsBOXF* first, GsBOXF* second);
int XOverlap(GsBOXF* first, GsBOXF* second);
int YOverlap(GsBOXF* first, GsBOXF* second);
int RectanglesTouch(GsBOXF* first, GsBOXF* second);
int XTouch(GsBOXF* first, GsBOXF* second);
int YTouch(GsBOXF* first, GsBOXF* second);
int FindWhereRectanglesTouch(GsBOXF* first, GsBOXF* second);
void DrawGameElements(int activeBuffer);
void DrawGameText(void);
int CheckLevelComplete (void);
void HandleGameOver (void);

// Rectangle for the background
GsBOXF BackgroundRectangle;

// Struct for a moving block
typedef struct {
	int dx;
	int dy;
	GsBOXF rectangle;
} MovingBlock;

// Struct for a static block with lives
typedef struct {
	int alive;
	GsBOXF rectangle;
} Block;

// Struct for a line border
typedef struct {
	GsLINE left;
	GsLINE right;
	GsLINE bottom;
	GsLINE top;
} Border;

// Game borders used for debugging
Border screenBorder;
Border gameAreaBorder;

// Game objects
MovingBlock bat;
MovingBlock ball;
Block blocks[MAX_BLOCKS]; 

// Variables for the gameplay
int score;
int lives;
int level;
int batWidth = BLOCK_WIDTH *2;
int frameNumber = 0;
int gamePaused = FALSE;
int pauseTimer = 0;

main() {
	int	activeBuffer;

	// Initialise all game elements
	InitialiseAllElements();
	
	// Run until the quit flag is set 
	while (QuitFlag == FALSE) {
	
		// Handle controller input
		HandleControllerInput();
		
		// Handle the bat, ball and blocks
		HandleAllObjects();

		// Get the active buffer
		activeBuffer = GetActiveBuffer ();

		// Draw the game elements
		DrawGameElements(activeBuffer);
		
		// Draw the game text
		DrawGameText();
		
		// Wait for the v-sync, swap display buffers, draw the ordering table
		UpdateGraphicsSystem (activeBuffer);
		
		frameNumber ++;
	}
}

//######################################################################
// Initialise functions

// Initialise the graphics
void InitialiseGraphicsSystem (void) {		 
	int	i;

	// First graphical initialisations
	GsInitGraph(320, 240, 4, 0, 0);	
	GsDefDispBuff(0, 0, 0, 240);
	
	// Set the screen offset (varies with PAL/NTSC)
	GsDISPENV.screen.x = SCREEN_X_OFFSET;
	GsDISPENV.screen.y = SCREEN_Y_OFFSET;	
	   
	// Set the ordering table
   	for (i = 0; i < 2; i++) {
		WorldOrderingTable[i].length = OT_LENGTH;
		WorldOrderingTable[i].org = OrderingTableTags[i];
	}
}

// Initialise all of the objects
void InitialiseGameObjects (void) {
	
	// Initialse the background rectangle
	InitialiseBackground();
	
	// Initialise all of the blocks
	InitialiseBlocks();
	
	// Initialise the bat
	InitialiseBat();
	
	// Initialise the ball
	InitialiseBall();
}

// Initialise the background
void InitialiseBackground (void) {
	BackgroundRectangle.x = 0;
	BackgroundRectangle.y = 0;
	BackgroundRectangle.w = SCREEN_WIDTH;
	BackgroundRectangle.h = SCREEN_HEIGHT;
	BackgroundRectangle.r = 0;
	BackgroundRectangle.g = 0;
	BackgroundRectangle.b = 0;
}

// Initialise the bat
void InitialiseBat (void) {
	bat.dx = 0;
	bat.dy = 0;
	bat.rectangle.x = SCREEN_WIDTH/2 - (batWidth/2);
	bat.rectangle.y = 225;
	bat.rectangle.w = batWidth;
	bat.rectangle.h = BLOCK_HEIGHT;
	bat.rectangle.r = 175;
	bat.rectangle.g = 175;
	bat.rectangle.b = 175;
}

// Initialise the ball
void InitialiseBall (void) {
	ball.dx = 0;
	ball.dy = 1;
	ball.rectangle.x = SCREEN_WIDTH/2 - (ball.rectangle.w/2);
	ball.rectangle.y = 120;
	ball.rectangle.w = 5;
	ball.rectangle.h = 5;
	ball.rectangle.r = 255;
	ball.rectangle.g = 255;
	ball.rectangle.b = 255;
}

// Initialise the blocks
void InitialiseBlocks (void) {
	int i;
	int red, green, blue;
	int x = MIN_X;
	int y = MIN_Y;
	
	// Sort out the blocks
	for (i = 0; i < MAX_BLOCKS; i++) {
	
		// Set the colours based on the row number
		if (i < 14) {
			red = 255;
			green = 0;
			blue = 0;
		}
		else if (i < 28) {
			red = 255;
			green = 150;
			blue = 0;
		}
		else if (i < 42) {
			red = 255;
			green = 255;
			blue = 0;
		}
		else if (i < 56) {
			red = 0;
			green = 255;
			blue = 0;
		}
		else if (i < 70) {
			red = 0;
			green = 150;
			blue = 255;
		}
	
		// Increment the y value when we reach the end of the row
		if ((i == 14) || (i == 28) || (i == 42) || (i == 56)) {
			y += 12;
			x = MIN_X;
		}
		
		// Initialise the blocks in the array
		blocks[i].alive = 1;
		blocks[i].rectangle.x = x;
		blocks[i].rectangle.y = y;
		blocks[i].rectangle.w = BLOCK_WIDTH;
		blocks[i].rectangle.h = BLOCK_HEIGHT;
		blocks[i].rectangle.r = red;
		blocks[i].rectangle.g = green;
		blocks[i].rectangle.b = blue;
		
		// Increment the x value
		x +=22;
	}
}
//######################################################################

//######################################################################
// Controller functions

// Set up low-level controller buffers
void InitialiseController (void) {
	GetPadBuf(&bb0, &bb1);
}

// Controller state reading 
static u_long PadRead(void) {
	return(~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));
}

// Handle controller input
void HandleControllerInput (void) {

	PadStatus = PadRead();
	
	// Bat move left
	if (PadStatus & PADLleft) {
		bat.dx -= BAT_SPEED;
		KeepWithinRange(bat.dx, -BAT_SPEED, BAT_SPEED);
	}
	
	// Bat move right
	if (PadStatus & PADLright) {
		bat.dx += BAT_SPEED;
		KeepWithinRange(bat.dx, -BAT_SPEED, BAT_SPEED);
	}
	
	// Quit
	if ((PadStatus & PADselect) && (PadStatus & PADstart)) {
		QuitFlag = TRUE;
		return;
	}
	
	// Pause
	if (PadStatus & PADstart) {
		if (frameNumber > (pauseTimer +8)) {
			if (gamePaused == FALSE) {
				gamePaused = TRUE;
				pauseTimer = frameNumber;
			}
			else{
				gamePaused = FALSE;
				pauseTimer = frameNumber;
			}
		}
	}
}
//######################################################################

//######################################################################
// Collision functions

// This checks if two rectangles overlap
int RectanglesOverlap (GsBOXF* first, GsBOXF* second) {
   	int boolean;
	boolean = XOverlap(first, second) && YOverlap(first, second);
	return boolean;
}

// This checks if two rectangles overlap on the x-axis
int XOverlap (GsBOXF* first, GsBOXF* second) {
	if (first->x >= second->x) {
		return (first->x < second->x + second->w);
	}
	else {
		return (first->x + first->w > second->x);
	}
}

// This checks if two rectangles overlap on the y-axis
int YOverlap (GsBOXF* first, GsBOXF* second) {
	if (first->y >= second->y) {
		return (first->y < second->y + second->h);
	}
	else {
		return (first->y + first->h > second->y);
	}
}

// This checks if two rectangles touch
int RectanglesTouch (GsBOXF* first, GsBOXF* second) {
	int boolean;
	boolean = (XTouch(first, second) && YOverlap(first, second)) || (YTouch(first, second) && XOverlap(first, second));
	return boolean;
}

// This checks if two rectangles touch on the x-axis
int XTouch (GsBOXF* first, GsBOXF* second) {
	if (first->x > second->x) {
		return (first->x == second->x + second->w);
	}
	else if (first->x == second->x) {
		return FALSE;
	}
	else {
		return (first->x + first->w == second->x);
	}
}

// This checks if two rectangles touch on the y-axis
int YTouch (GsBOXF* first, GsBOXF* second) {
	if (first->y > second->y) {
		return (first->y == second->y + second->h);
	}
	else if (first->y == second->y) {
		return FALSE;
	}
  	else {
		return (first->y + first->h == second->y);
	}
}

// Determine which part touches on the x-axis
int GetXtouch(GsBOXF* first, GsBOXF* second) {
	if (first->x > second->x) {
		if (first->x == second->x + second->w)
			return LEFT;
		else if (first->x < second->x + second->w)
			return MIDDLE;	
		else
			return NONE;
	}
	else if (first->x == second->x)
		return MIDDLE;
	else {
		if (first->x + first->w == second->x)
			return RIGHT;
		else if (first->x + first->w > second->x)
			return MIDDLE;
		else
			return NONE;
	}
}

// Determine which part touches on the y-axis
int GetYtouch(GsBOXF* first, GsBOXF* second) {
	if (first->y > second->y) {
		if (first->y == second->y + second->h)
			return TOP;
		else if (first->y < second->y + second->h)
			return MIDDLE;	
		else
			return NONE;
	}
	else if (first->y == second->y)
		return MIDDLE;
	else {
		if (first->y + first->h == second->y)
			return BOTTOM;
		else if (first->y + first->h > second->y)
			return MIDDLE;
		else
			return NONE;
	}
}

// This checks which part of the rectangle touch
int FindWhereRectanglesTouch (GsBOXF* first, GsBOXF* second) {
	int whereTheyTouch;							
	int xTouch;		
	int yTouch;		

	// Get the parts that touch
	xTouch = GetXtouch(first, second);
	yTouch = GetYtouch(first, second);

	switch(xTouch) {
		case LEFT:
			switch(yTouch) {
				case TOP:
					whereTheyTouch = TOP_LEFT;
					break;
				case BOTTOM:
					whereTheyTouch = BOTTOM_LEFT;
					break;
				case MIDDLE:
					whereTheyTouch = LEFT;
					break;
				case NONE:
					whereTheyTouch = NONE;
					break;
			}
			break;
		case RIGHT:
			switch(yTouch) {
				case TOP:
					whereTheyTouch = TOP_RIGHT;
					break;
				case BOTTOM:
					whereTheyTouch = BOTTOM_RIGHT;
					break;
				case MIDDLE:
					whereTheyTouch = RIGHT;
					break;
				case NONE:
					whereTheyTouch = NONE;
					break;
			}
			break;
		case MIDDLE:
			switch(yTouch) {
				case TOP:
					whereTheyTouch = TOP;
					break;
				case BOTTOM:
					whereTheyTouch = BOTTOM;
					break;
				case MIDDLE:
					whereTheyTouch = OVERLAP;
					break;
				case NONE:
					whereTheyTouch = NONE;
					break;
			}
			break;	
		case NONE:
			whereTheyTouch = NONE;
			break;
	}
		 
	return whereTheyTouch;
}

// Handle collisions between the bat and the ball
void HandleBatHitBall (void) {

	if (RectanglesOverlap(&bat.rectangle, &ball.rectangle)) {
		MoveBallOutOfBat();
	}
	
	// If below top-of-bat line, reflect off side of bat
	if (ball.rectangle.y > (MAX_Y-bat.rectangle.h - ball.rectangle.h)) {
	
		if (ball.dx == 0) {
			ball.dx = bat.dx;
		}
		else if (SIGN_OF(ball.dx) == SIGN_OF(bat.dx)) {
			ball.dx += (bat.dx);
		}
		else {
			ball.dx = -ball.dx + (bat.dx);
		}
	}
	else {
		// First: reflect off top of bat
		ball.dy = -ball.dy;
		ball.dx += (bat.dx/2);
	}

	// Restrict the ball's velocity
	KeepWithinRange(ball.dx, -MAX_BALL_X_SPEED, MAX_BALL_X_SPEED);
	KeepWithinRange(ball.dy, -MAX_BALL_Y_SPEED, MAX_BALL_Y_SPEED);
	ball.rectangle.x += ball.dx;
	ball.rectangle.y += ball.dy;
}

// Handle collisions between the ball and the blocks
void HandleBallHitBlock (int i) {
	int edge;	
	int xInitialDistance, xLeftOver;
	int yInitialDistance, yLeftOver;
		
	edge = FindWhereRectanglesTouch(&ball.rectangle, &blocks[i].rectangle);

	// Deal with reflection and move in one go
	switch(edge) {
		case LEFT:
			xInitialDistance = ball.rectangle.x - (blocks[i].rectangle.x + blocks[i].rectangle.w);
			xLeftOver = -ball.dx - xInitialDistance;
			ball.rectangle.x += (ball.dx + (2 * xLeftOver));
			ball.dx = -ball.dx;
			ball.rectangle.y += ball.dy;
			break;
		case RIGHT:
			xInitialDistance = blocks[i].rectangle.x - (ball.rectangle.x + ball.rectangle.w);
			xLeftOver = ball.dx - xInitialDistance;
			ball.rectangle.x += (ball.dx - (2 * xLeftOver));
			ball.dx = -ball.dx;
			ball.rectangle.y += ball.dy;
			break;
		case TOP:
		case TOP_LEFT:
		case TOP_RIGHT:
			yInitialDistance = ball.rectangle.y - (blocks[i].rectangle.y + blocks[i].rectangle.h);
			yLeftOver = -ball.dy - yInitialDistance;
			ball.rectangle.y += (ball.dy + (2 * yLeftOver));
			ball.dy = -ball.dy;
			ball.rectangle.x += ball.dx;
			break;
		case BOTTOM:
		case BOTTOM_RIGHT:
		case BOTTOM_LEFT:
			yInitialDistance = blocks[i].rectangle.y - (ball.rectangle.y + ball.rectangle.h);
			yLeftOver = ball.dy - yInitialDistance;
			ball.rectangle.y += (ball.dy - (2 * yLeftOver));
			ball.dy = -ball.dy;
			ball.rectangle.x += ball.dx;
			break;
		default:
			assert(FALSE);
		}

}

// Handle collisions between the ball and the walls/ceiling
void HandleBallHitWall (void) {
	if (ball.rectangle.x > MAX_X-ball.rectangle.w) {
		ball.rectangle.x -= (2 * (ball.rectangle.x - (MAX_X-ball.rectangle.w)) );
		ball.dx = -ball.dx;
	}
	if (ball.rectangle.x < MIN_X) {
		ball.rectangle.x += (2 * (MIN_X - ball.rectangle.x));
		ball.dx = -ball.dx;
	}
	if (ball.rectangle.y < MIN_Y) {
		ball.rectangle.y += (2 * (MIN_Y - ball.rectangle.y));
		ball.dy = -ball.dy;
	}	
}
//######################################################################

// Handle the bat
void HandleBat (void) {
	bat.rectangle.x += bat.dx;

	// Deny momentum to the bat
	bat.dx = 0;

	// Bat stops at walls
	if (bat.rectangle.x > MAX_X - bat.rectangle.w) {
		bat.rectangle.x = MAX_X - bat.rectangle.w;
		bat.dx = 0;
	}
	else if (bat.rectangle.x < MIN_X) {
		bat.rectangle.x = MIN_X;
		bat.dx = 0;
	}
}

// Handle all of the objects
void HandleAllObjects (void) {

	int levelComplete;

	if (gamePaused == FALSE) {
		int ballMoved = FALSE;	
		int i;

		// Check if the bat collides with the ball
		if (RectanglesTouch(&bat.rectangle, &ball.rectangle) || RectanglesOverlap(&bat.rectangle, &ball.rectangle)) {
			HandleBatHitBall();
			ballMoved = TRUE;
		}
		else {
			// Check if the ball collides with any of the blocks
			for (i = 0; i < MAX_BLOCKS; i++) {
				if (blocks[i].alive == TRUE) {		
					if (RectanglesOverlap(&ball.rectangle, &blocks[i].rectangle) || RectanglesTouch(&ball.rectangle, &blocks[i].rectangle)) {
						HandleBallHitBlock(i);
						ballMoved = TRUE;
						DestroySingleBlock(i);
						
						// Increment the score
						if (level ==1){
							score +=1;
						}
					}
				}
			}
		}

		// keep the ball's velocity within a sensible range
		KeepWithinRange(ball.dx, -MAX_BALL_X_SPEED, MAX_BALL_X_SPEED);
		KeepWithinRange(ball.dy, -MAX_BALL_Y_SPEED, MAX_BALL_Y_SPEED);
					
		if (ballMoved == FALSE)	{  				
			// Update the balls position
			ball.rectangle.x += ball.dx;
			ball.rectangle.y += ball.dy;
		}

		HandleBallHitWall();
		
		if (ball.rectangle.y > MAX_Y-ball.rectangle.h) {
			lives--;
			Delay(30);		
			InitialiseBall();
			InitialiseBat();
		}
		
		if (lives == -1){
			HandleGameOver();
		}
		   
		HandleBat();
		
		// Check if all of the blocks have been destroyed
		levelComplete = CheckLevelComplete();
	}
}

// Handle gameover
void HandleGameOver (void) {
	
}

// Check if all of the blocks have been destroyed
int CheckLevelComplete (void) {
	int i;
	int allBlocksDestroyed = TRUE;
	for (i = 0; i < MAX_BLOCKS; i++) {
		if (blocks[i].alive == TRUE){
			allBlocksDestroyed = FALSE;
		}
	}
	return allBlocksDestroyed;
}

// Draws the score and lives text
void DrawGameText (void) {

	// Load the font
	FntLoad(960, 256);	
	
	// Open the font
	FntOpen(7, 3, 350, 50, 0, 50);
	
	// Print the lives and score text
	FntPrint("Lives: %d                   ", lives);
	FntPrint("Score: %d", score);
	
	// Flush the font
	FntFlush(-1);
}

// Draws the screen border for debugging
void DrawScreenBorder (int activeBuffer) {
	screenBorder.left.x0 = 0;
	screenBorder.left.y0 = 0;
	screenBorder.left.x1 = 0;
	screenBorder.left.y1 = SCREEN_HEIGHT -1;
	screenBorder.left.r = 255;
	screenBorder.left.g = 0;
	screenBorder.left.b = 0;
	GsSortLine(&screenBorder.left, &WorldOrderingTable[activeBuffer], 0);
	
	screenBorder.right.x0 = SCREEN_WIDTH -1;
	screenBorder.right.y0 = 0;
	screenBorder.right.x1 = SCREEN_WIDTH -1;
	screenBorder.right.y1 = SCREEN_HEIGHT -1;
	screenBorder.right.r = 255;
	screenBorder.right.g = 0;
	screenBorder.right.b = 0;
	GsSortLine(&screenBorder.right, &WorldOrderingTable[activeBuffer], 0);
	
	screenBorder.bottom.x0 = 0;
	screenBorder.bottom.y0 = SCREEN_HEIGHT -1;
	screenBorder.bottom.x1 = SCREEN_WIDTH -1;
	screenBorder.bottom.y1 = SCREEN_HEIGHT -1;
	screenBorder.bottom.r = 255;
	screenBorder.bottom.g = 0;
	screenBorder.bottom.b = 0;
	GsSortLine(&screenBorder.bottom, &WorldOrderingTable[activeBuffer], 0);
	
	screenBorder.top.x0 = 0;
	screenBorder.top.y0 = 0;
	screenBorder.top.x1 = SCREEN_WIDTH -1;
	screenBorder.top.y1 = 0;
	screenBorder.top.r = 255;
	screenBorder.top.g = 0;
	screenBorder.top.b = 0;
	GsSortLine(&screenBorder.top, &WorldOrderingTable[activeBuffer], 0);
}

// Draws the game area border for debugging
void DrawGameAreaBorder (int activeBuffer) {
	gameAreaBorder.left.x0 = MIN_X;
	gameAreaBorder.left.y0 = MIN_Y;
	gameAreaBorder.left.x1 = MIN_X;
	gameAreaBorder.left.y1 = MAX_Y;
	gameAreaBorder.left.r = 0;
	gameAreaBorder.left.g = 0;
	gameAreaBorder.left.b = 255;
	GsSortLine(&gameAreaBorder.left, &WorldOrderingTable[activeBuffer], 0);
	
	gameAreaBorder.right.x0 = MAX_X-1;
	gameAreaBorder.right.y0 = MIN_Y;
	gameAreaBorder.right.x1 = MAX_X-1;
	gameAreaBorder.right.y1 = MAX_Y;
	gameAreaBorder.right.r = 0;
	gameAreaBorder.right.g = 0;
	gameAreaBorder.right.b = 255;
	GsSortLine(&gameAreaBorder.right, &WorldOrderingTable[activeBuffer], 0);
	
	gameAreaBorder.bottom.x0 = MIN_X;
	gameAreaBorder.bottom.y0 = MAX_Y;
	gameAreaBorder.bottom.x1 = MAX_X-1;
	gameAreaBorder.bottom.y1 = MAX_Y;
	gameAreaBorder.bottom.r = 0;
	gameAreaBorder.bottom.g = 0;
	gameAreaBorder.bottom.b = 255;
	GsSortLine(&gameAreaBorder.bottom, &WorldOrderingTable[activeBuffer], 0);
	
	gameAreaBorder.top.x0 = MIN_X;
	gameAreaBorder.top.y0 = MIN_Y;
	gameAreaBorder.top.x1 = MAX_X-1;
	gameAreaBorder.top.y1 = MIN_Y;
	gameAreaBorder.top.r = 0;
	gameAreaBorder.top.g = 0;
	gameAreaBorder.top.b = 255;
	GsSortLine(&gameAreaBorder.top, &WorldOrderingTable[activeBuffer], 0);
}

// Draw the game elements
void DrawGameElements (int activeBuffer) {
	int	i;

	// Draw the black background
	GsSortBoxFill(&BackgroundRectangle, &WorldOrderingTable[activeBuffer], 1);

	// Draw all of the alive rectangles fron the array
	for (i = 0; i < MAX_BLOCKS; i++) {
		if(blocks[i].alive ==1) {
			GsSortBoxFill(&blocks[i].rectangle, &WorldOrderingTable[activeBuffer], 0);
		}
	}
	
	// Draw the bat
	GsSortBoxFill(&bat.rectangle, &WorldOrderingTable[activeBuffer], 0);
	
	// Draw the ball
	GsSortBoxFill(&ball.rectangle, &WorldOrderingTable[activeBuffer], 0);
	
	// Draw the game borders for debugging purposes
	//DrawScreenBorder(activeBuffer);
	//DrawGameAreaBorder(activeBuffer);
}

// Destroy a single block
void DestroySingleBlock (int i) {
	blocks[i].alive = 0;
}

// Move the ball upwards away from the bat
void MoveBallOutOfBat (void) {
	if (((int)ball.rectangle.y) + ((int)ball.rectangle.h) > ((int)bat.rectangle.y)) {
		ball.rectangle.y = bat.rectangle.y - ball.rectangle.h;
	}
}

// Creates a short delay
void Delay (int n){
	int i;
	for (i = 0; i < n; i++) {
		VSync(0);
	}
}

// Get the active buffer and clear the ordering table
int GetActiveBuffer (void) {
	int	activeBuffer;

	// Get the active buffer
	activeBuffer = GsGetActiveBuff();
	GsSetWorkBase((PACKET *) GpuPacketArea[activeBuffer]);
	
	// Clear the graphics ordering table
	GsClearOt(0, 0, &WorldOrderingTable[activeBuffer]);
	
	return activeBuffer;
}

// Update the graphics system
void UpdateGraphicsSystem (int activeBuffer) {
	// Wait for the vertical sync
	DrawSync(0);							
	VSync(0);
	
	// Swap the display buffer		
	GsSwapDispBuff();		

	GsSortClear(60, 120, 120, &WorldOrderingTable[activeBuffer]);
	
	// Draw the order tablebdata
	GsDrawOt(&WorldOrderingTable[activeBuffer]);
}

void SetInitialGameValues () {
	level = 1;
	score = 0;
	lives = 5;
}

// Initialise the graphics, control pad and game objects
void InitialiseAllElements (void) {
	// Initialise the graphics
	InitialiseGraphicsSystem();
	
	// Initialise the control pad
	InitialiseController();
	
	// Initialse the background, bat, ball and blocks
	InitialiseGameObjects();
	
	// Set the initial game values
	SetInitialGameValues ();
}