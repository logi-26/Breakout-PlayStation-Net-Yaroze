//*****************************************************************
// Most of the functions and variables are defined in this class
//*****************************************************************

#ifndef MAIN_H
#define MAIN_H

// Import Net Yaroze libraries
#include <libps.h>
#include "Pad.h"

// Import C libraries
#include <stdio.h>
#include <string.h>

#include "myassert.h"

// Scaling factor for fixed-point arithmetic
#define FIXED_POINT_SCALE 4

// Constants
#define TRUE 1
#define FALSE 0

#define NONE 32766
#define SIGN_OF(number) (((number) >= 0) ? 1 : -1)
#define OT_LENGTH 1    

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

#define KeepWithinRange(x, l, h) ((x) = ((x) < (l) ? (l) : (x) > (h) ? (h) : (x)))    

// Ordering table related variables
extern GsOT WorldOrderingTable[2];
extern GsOT_TAG OrderingTableTags[2][1 << OT_LENGTH];
extern PACKET GpuPacketArea[2][MAX_OBJECTS * MAX_SIZEOF_PRIMITIVE];

// Storage for controller information
extern volatile u_char *bb0, *bb1;

// Controller pad buffer
extern u_long PadStatus;

// Check if user wants to quit
extern int QuitFlag;

// Game state enumeration
enum gameState { titleScreen, playing, ended, paused };

// Accumulators for the fixed point interger math
extern int x_accumulator;
extern int y_accumulator;

// Rectangle for the background
extern GsBOXF BackgroundRectangle;

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

// Declare the random seed variable
extern unsigned int seed;

// Game borders used for debugging
extern Border screenBorder;
extern Border gameAreaBorder;

// Game objects
extern MovingBlock bat;
extern MovingBlock ball;
extern Block blocks[MAX_BLOCKS]; 

// Variables for the gameplay
extern int score;
extern int lives;
extern int level;
extern int batWidth;
extern int frameNumber;
extern int gamePaused;
extern int pauseTimer;

// Function prototypes
int main(void);
int GetActiveBuffer(void);
void InitialiseGraphicsSystem(void);
void InitialiseAllElements(void);
void InitialiseController(void);
void InitialiseBackground(void);
void InitialiseBlocks(void);
void InitialiseBat(void);
void InitialiseBall(void);
void InitialiseGameObjects(void);
void SetInitialGameValues(void);
void UpdateGraphicsSystem(int activeBuffer);
u_long PadRead(void);
void HandleControllerInput(void);
void HandleAllObjects(void);
void MoveBallOutOfBat(void);
void DestroySingleBlock(int);
int RectanglesOverlap(GsBOXF* first, GsBOXF* second);
int XOverlap(GsBOXF* first, GsBOXF* second);
int YOverlap(GsBOXF* first, GsBOXF* second);
int RectanglesTouch(GsBOXF* first, GsBOXF* second);
int XTouch(GsBOXF* first, GsBOXF* second);
int YTouch(GsBOXF* first, GsBOXF* second);
int FindWhereRectanglesTouch(GsBOXF* first, GsBOXF* second);
void DrawGameElements(int activeBuffer);
void DrawGameText(void);
int CheckLevelComplete(void);
void HandleGameOver(void);

#endif // MAIN_H