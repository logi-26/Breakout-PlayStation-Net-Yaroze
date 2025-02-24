#include "render.h"
#include "game.h"

// Draw the game elements
void DrawGameElements(int activeBuffer) {
    int i;

    // Draw the black background
    GsSortBoxFill(&BackgroundRectangle, &WorldOrderingTable[activeBuffer], 1);

    // Draw all of the alive rectangles from the array
    for (i = 0; i < MAX_BLOCKS; i++) {
        if (blocks[i].alive == 1) {
            GsSortBoxFill(&blocks[i].rectangle, &WorldOrderingTable[activeBuffer], 0);
        }
    }
    
    // Draw the bat
    GsSortBoxFill(&bat.rectangle, &WorldOrderingTable[activeBuffer], 0);
    
    // Draw the ball
    GsSortBoxFill(&ball.rectangle, &WorldOrderingTable[activeBuffer], 0);
    
    // Draw the game borders for debugging purposes
    // DrawScreenBorder(activeBuffer);
    // DrawGameAreaBorder(activeBuffer);
}

// Draws the score and lives text
void DrawGameText(void) {
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
void DrawScreenBorder(int activeBuffer) {
    screenBorder.left.x0 = 0;
    screenBorder.left.y0 = 0;
    screenBorder.left.x1 = 0;
    screenBorder.left.y1 = SCREEN_HEIGHT - 1;
    screenBorder.left.r = 255;
    screenBorder.left.g = 0;
    screenBorder.left.b = 0;
    GsSortLine(&screenBorder.left, &WorldOrderingTable[activeBuffer], 0);
    
    screenBorder.right.x0 = SCREEN_WIDTH - 1;
    screenBorder.right.y0 = 0;
    screenBorder.right.x1 = SCREEN_WIDTH - 1;
    screenBorder.right.y1 = SCREEN_HEIGHT - 1;
    screenBorder.right.r = 255;
    screenBorder.right.g = 0;
    screenBorder.right.b = 0;
    GsSortLine(&screenBorder.right, &WorldOrderingTable[activeBuffer], 0);
    
    screenBorder.bottom.x0 = 0;
    screenBorder.bottom.y0 = SCREEN_HEIGHT - 1;
    screenBorder.bottom.x1 = SCREEN_WIDTH - 1;
    screenBorder.bottom.y1 = SCREEN_HEIGHT - 1;
    screenBorder.bottom.r = 255;
    screenBorder.bottom.g = 0;
    screenBorder.bottom.b = 0;
    GsSortLine(&screenBorder.bottom, &WorldOrderingTable[activeBuffer], 0);
    
    screenBorder.top.x0 = 0;
    screenBorder.top.y0 = 0;
    screenBorder.top.x1 = SCREEN_WIDTH - 1;
    screenBorder.top.y1 = 0;
    screenBorder.top.r = 255;
    screenBorder.top.g = 0;
    screenBorder.top.b = 0;
    GsSortLine(&screenBorder.top, &WorldOrderingTable[activeBuffer], 0);
}

// Draws the game area border for debugging
void DrawGameAreaBorder(int activeBuffer) {
    gameAreaBorder.left.x0 = MIN_X;
    gameAreaBorder.left.y0 = MIN_Y;
    gameAreaBorder.left.x1 = MIN_X;
    gameAreaBorder.left.y1 = MAX_Y;
    gameAreaBorder.left.r = 0;
    gameAreaBorder.left.g = 0;
    gameAreaBorder.left.b = 255;
    GsSortLine(&gameAreaBorder.left, &WorldOrderingTable[activeBuffer], 0);
    
    gameAreaBorder.right.x0 = MAX_X - 1;
    gameAreaBorder.right.y0 = MIN_Y;
    gameAreaBorder.right.x1 = MAX_X - 1;
    gameAreaBorder.right.y1 = MAX_Y;
    gameAreaBorder.right.r = 0;
    gameAreaBorder.right.g = 0;
    gameAreaBorder.right.b = 255;
    GsSortLine(&gameAreaBorder.right, &WorldOrderingTable[activeBuffer], 0);
    
    gameAreaBorder.bottom.x0 = MIN_X;
    gameAreaBorder.bottom.y0 = MAX_Y;
    gameAreaBorder.bottom.x1 = MAX_X - 1;
    gameAreaBorder.bottom.y1 = MAX_Y;
    gameAreaBorder.bottom.r = 0;
    gameAreaBorder.bottom.g = 0;
    gameAreaBorder.bottom.b = 255;
    GsSortLine(&gameAreaBorder.bottom, &WorldOrderingTable[activeBuffer], 0);
    
    gameAreaBorder.top.x0 = MIN_X;
    gameAreaBorder.top.y0 = MIN_Y;
    gameAreaBorder.top.x1 = MAX_X - 1;
    gameAreaBorder.top.y1 = MIN_Y;
    gameAreaBorder.top.r = 0;
    gameAreaBorder.top.g = 0;
    gameAreaBorder.top.b = 255;
    GsSortLine(&gameAreaBorder.top, &WorldOrderingTable[activeBuffer], 0);
}