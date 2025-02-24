#ifndef LOGIC_H
#define LOGIC_H

void HandleControllerInput(void);
void HandleAllObjects(void);
void HandleBatHitBall(void);
void HandleBallHitBlock(int i);
void HandleBallHitWall(void);
void HandleBat(void);
void HandleGameOver(void);
int CheckLevelComplete(void);

#endif