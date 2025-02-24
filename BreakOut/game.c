#include "game.h"

// Define global variables
GsOT WorldOrderingTable[2];
GsOT_TAG OrderingTableTags[2][1 << OT_LENGTH];
PACKET GpuPacketArea[2][MAX_OBJECTS * MAX_SIZEOF_PRIMITIVE];

// Initialise the random seed variable
unsigned int seed = 696969;

int x_accumulator = 0;
int y_accumulator = 0;

volatile u_char *bb0, *bb1;
u_long PadStatus;
int QuitFlag = FALSE;

GsBOXF BackgroundRectangle;
Border screenBorder;
Border gameAreaBorder;

MovingBlock bat;
MovingBlock ball;
Block blocks[MAX_BLOCKS];

int score = 0;
int lives = 5;
int level = 1;
int batWidth = BLOCK_WIDTH * 2;
int frameNumber = 0;
int gamePaused = FALSE;
int pauseTimer = 0;

// Function to move the ball upwards away from the bat
void MoveBallOutOfBat(void) {
    if (((int)ball.rectangle.y) + ((int)ball.rectangle.h) > ((int)bat.rectangle.y)) {
        ball.rectangle.y = bat.rectangle.y - ball.rectangle.h;
    }
}

// Function to destroy a single block
void DestroySingleBlock(int i) {
    blocks[i].alive = 0;
}

// Function to get the active buffer and clear the ordering table
int GetActiveBuffer(void) {
    int activeBuffer;

    // Get the active buffer
    activeBuffer = GsGetActiveBuff();
    GsSetWorkBase((PACKET *) GpuPacketArea[activeBuffer]);
    
    // Clear the graphics ordering table
    GsClearOt(0, 0, &WorldOrderingTable[activeBuffer]);
    
    return activeBuffer;
}

// Function to update the graphics system
void UpdateGraphicsSystem(int activeBuffer) {
    // Wait for the vertical sync
    DrawSync(0);                            
    VSync(0);
    
    // Swap the display buffer        
    GsSwapDispBuff();        

    GsSortClear(60, 120, 120, &WorldOrderingTable[activeBuffer]);
    
    // Draw the ordering table data
    GsDrawOt(&WorldOrderingTable[activeBuffer]);
}

// Controller state reading 
u_long PadRead(void) {
    return (~(*(bb0 + 3) | *(bb0 + 2) << 8 | *(bb1 + 3) << 16 | *(bb1 + 2) << 24));
}