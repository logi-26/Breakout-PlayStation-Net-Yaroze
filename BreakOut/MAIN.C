#include "game.h"

int main(void) {
    int activeBuffer;

    // Initialise all game elements
    InitialiseAllElements();
    
    // Run until the quit flag is set 
    while (QuitFlag == FALSE) {
    
        // Handle controller input
        HandleControllerInput();
        
        // Handle the bat, ball and blocks
        HandleAllObjects();

        // Get the active buffer
        activeBuffer = GetActiveBuffer();

        // Draw the game elements
        DrawGameElements(activeBuffer);
        
        // Draw the game text
        DrawGameText();
        
        // Wait for the v-sync, swap display buffers, draw the ordering table
        UpdateGraphicsSystem(activeBuffer);
        
        frameNumber++;
    }

    return 0;
}