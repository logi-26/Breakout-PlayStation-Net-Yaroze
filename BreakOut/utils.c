#include "utils.h"
#include "game.h"

// Pseudo-random number generator
unsigned int RandomNumber(void) {
    seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF;
    return seed;
}

// Function to create a short delay
void Delay(int n) {
    int i;
    for (i = 0; i < n; i++) {
        VSync(0);
    }
}