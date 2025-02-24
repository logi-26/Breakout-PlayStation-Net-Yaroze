#include "init.h"
#include "game.h"

#include <libps.h> 

// Initialise the graphics system
void InitialiseGraphicsSystem(void) {
    int i;

    // First graphical initialisations
    GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, 4, 0, 0);    
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
void InitialiseGameObjects(void) {
    // Initialise the background rectangle
    InitialiseBackground();
    
    // Initialise all of the blocks
    InitialiseBlocks();
    
    // Initialise the bat
    InitialiseBat();
    
    // Initialise the ball
    InitialiseBall();
}

// Initialise the background
void InitialiseBackground(void) {
    BackgroundRectangle.x = 0;
    BackgroundRectangle.y = 0;
    BackgroundRectangle.w = SCREEN_WIDTH;
    BackgroundRectangle.h = SCREEN_HEIGHT;
    BackgroundRectangle.r = 0;
    BackgroundRectangle.g = 0;
    BackgroundRectangle.b = 0;
}

// Initialise the bat
void InitialiseBat(void) {
    bat.dx = 0;
    bat.dy = 0;
    bat.rectangle.x = SCREEN_WIDTH / 2 - (batWidth / 2);
    bat.rectangle.y = 225;
    bat.rectangle.w = batWidth;
    bat.rectangle.h = BLOCK_HEIGHT;
    bat.rectangle.r = 175;
    bat.rectangle.g = 175;
    bat.rectangle.b = 175;
}

// Initialise the ball
void InitialiseBall(void) {
    int directionX;
    unsigned int randomValue;

    // Generate a random number
    randomValue = RandomNumber();

    // Randomly set the horizontal direction (left or right)
    directionX = (randomValue % 2) ? 1 : -1;

    // Set initial velocities
    ball.dx = directionX * 10 * FIXED_POINT_SCALE;  // Random horizontal direction
    ball.dy = 10 * FIXED_POINT_SCALE;               // Fixed vertical direction (downward)

    // Set initial position
    ball.rectangle.x = SCREEN_WIDTH / 2 - (ball.rectangle.w / 2);
    ball.rectangle.y = 120;
    ball.rectangle.w = 5;
    ball.rectangle.h = 5;
    ball.rectangle.r = 255;
    ball.rectangle.g = 255;
    ball.rectangle.b = 255;

    // Reset accumulators
    x_accumulator = 0;
    y_accumulator = 0;
}

// Initialise the blocks
void InitialiseBlocks(void) {
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
        } else if (i < 28) {
            red = 255;
            green = 150;
            blue = 0;
        } else if (i < 42) {
            red = 255;
            green = 255;
            blue = 0;
        } else if (i < 56) {
            red = 0;
            green = 255;
            blue = 0;
        } else if (i < 70) {
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
        x += 22;
    }
}

// Set up low-level controller buffers
void InitialiseController(void) {
    GetPadBuf(&bb0, &bb1);
}

// Set initial game values
void SetInitialGameValues(void) {
    level = 1;
    score = 0;
    lives = 5;
}

// Initialise the graphics, control pad, and game objects
void InitialiseAllElements(void) {
    // Initialise the graphics
    InitialiseGraphicsSystem();
    
    // Initialise the control pad
    InitialiseController();
    
    // Initialise the background, bat, ball, and blocks
    InitialiseGameObjects();
    
    // Set the initial game values
    SetInitialGameValues();
}