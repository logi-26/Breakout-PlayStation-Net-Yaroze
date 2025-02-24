#include "logic.h"
#include "game.h"

// Handle controller input
void HandleControllerInput(void) {
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
        if (frameNumber > (pauseTimer + 8)) {
            if (gamePaused == FALSE) {
                gamePaused = TRUE;
                pauseTimer = frameNumber;
            } else {
                gamePaused = FALSE;
                pauseTimer = frameNumber;
            }
        }
    }
}

// Handle collisions between the bat and the ball
void HandleBatHitBall(void) {
    if (RectanglesOverlap(&bat.rectangle, &ball.rectangle)) {
        MoveBallOutOfBat();
    }
    
    // If below top-of-bat line, reflect off side of bat
    if (ball.rectangle.y > (MAX_Y - bat.rectangle.h - ball.rectangle.h)) {
        if (ball.dx == 0) {
            ball.dx = bat.dx;
        } else if (SIGN_OF(ball.dx) == SIGN_OF(bat.dx)) {
            ball.dx += (bat.dx);
        } else {
            ball.dx = -ball.dx + (bat.dx);
        }
    } else {
        // First: reflect off top of bat
        ball.dy = -ball.dy;
        ball.dx += (bat.dx / 2);
    }

    // Restrict the ball's velocity
    KeepWithinRange(ball.dx, -MAX_BALL_X_SPEED, MAX_BALL_X_SPEED);
    KeepWithinRange(ball.dy, -MAX_BALL_Y_SPEED, MAX_BALL_Y_SPEED);
    ball.rectangle.x += ball.dx;
    ball.rectangle.y += ball.dy;
}

// Handle collisions between the ball and the blocks
void HandleBallHitBlock(int i) {
    int edge;
    int xInitialDistance, xLeftOver;
    int yInitialDistance, yLeftOver;

    edge = FindWhereRectanglesTouch(&ball.rectangle, &blocks[i].rectangle);

    // Handle overlap
    if (edge == OVERLAP) {
        // Move the ball out of the block to prevent sticking
        if (ball.dx > 0) ball.rectangle.x = blocks[i].rectangle.x - ball.rectangle.w;
        else if (ball.dx < 0) ball.rectangle.x = blocks[i].rectangle.x + blocks[i].rectangle.w;
        if (ball.dy > 0) ball.rectangle.y = blocks[i].rectangle.y - ball.rectangle.h;
        else if (ball.dy < 0) ball.rectangle.y = blocks[i].rectangle.y + blocks[i].rectangle.h;
        edge = FindWhereRectanglesTouch(&ball.rectangle, &blocks[i].rectangle); // Re-check the edge
    }

    // Deal with reflection and move in one go
    switch (edge) {
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
            // Fallback behavior for unexpected cases
            ball.dx = -ball.dx;
            ball.dy = -ball.dy;
            break;
    }

    // Destroy the block
    blocks[i].active = FALSE;
}

// Handle collisions between the ball and the walls/ceiling
void HandleBallHitWall(void) {
    if (ball.rectangle.x > MAX_X - ball.rectangle.w) {
        ball.rectangle.x -= (2 * (ball.rectangle.x - (MAX_X - ball.rectangle.w)));
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

// Handle the bat
void HandleBat(void) {
    bat.rectangle.x += bat.dx;

    // Deny momentum to the bat
    bat.dx = 0;

    // Bat stops at walls
    if (bat.rectangle.x > MAX_X - bat.rectangle.w) {
        bat.rectangle.x = MAX_X - bat.rectangle.w;
        bat.dx = 0;
    } else if (bat.rectangle.x < MIN_X) {
        bat.rectangle.x = MIN_X;
        bat.dx = 0;
    }
}

// Handle all of the objects
void HandleAllObjects(void) {
    int levelComplete;

    if (gamePaused == FALSE) {
        int ballMoved = FALSE;    
        int i;

        // Check if the bat collides with the ball
        if (RectanglesTouch(&bat.rectangle, &ball.rectangle) || RectanglesOverlap(&bat.rectangle, &ball.rectangle)) {
            HandleBatHitBall();
            ballMoved = TRUE;
        } else {
            // Check if the ball collides with any of the blocks
            for (i = 0; i < MAX_BLOCKS; i++) {
                if (blocks[i].alive == TRUE) {        
                    if (RectanglesOverlap(&ball.rectangle, &blocks[i].rectangle) || RectanglesTouch(&ball.rectangle, &blocks[i].rectangle)) {
                        HandleBallHitBlock(i);
                        ballMoved = TRUE;
                        DestroySingleBlock(i);
                        
                        // Increment the score
                        if (level == 1) {
                            score += 1;
                        }
                    }
                }
            }
        }

        // Keep the ball's velocity within a sensible range
        KeepWithinRange(ball.dx, -MAX_BALL_X_SPEED, MAX_BALL_X_SPEED);
        KeepWithinRange(ball.dy, -MAX_BALL_Y_SPEED, MAX_BALL_Y_SPEED);
                    
        if (ballMoved == FALSE) {                
            // Update the ball's position using fixed-point arithmetic
            x_accumulator += ball.dx;
            y_accumulator += ball.dy;

            // Move the ball when the accumulator exceeds the scaling factor
            ball.rectangle.x += x_accumulator / FIXED_POINT_SCALE;
            ball.rectangle.y += y_accumulator / FIXED_POINT_SCALE;

            // Keep the remainder in the accumulator
            x_accumulator %= FIXED_POINT_SCALE;
            y_accumulator %= FIXED_POINT_SCALE;
        }

        HandleBallHitWall();
        
        if (ball.rectangle.y > MAX_Y - ball.rectangle.h) {
            lives--;
            Delay(30);        
            InitialiseBall();
            InitialiseBat();
        }
        
        if (lives == -1) {
            HandleGameOver();
        }
           
        HandleBat();
        
        // Check if all of the blocks have been destroyed
        levelComplete = CheckLevelComplete();
    }
}

// Handle game over
void HandleGameOver(void) {
    // Game over logic (e.g., display game over screen, reset game, etc.)
}

// Check if all of the blocks have been destroyed
int CheckLevelComplete(void) {
    int i;
    int allBlocksDestroyed = TRUE;
    for (i = 0; i < MAX_BLOCKS; i++) {
        if (blocks[i].alive == TRUE) {
            allBlocksDestroyed = FALSE;
        }
    }
    return allBlocksDestroyed;
}