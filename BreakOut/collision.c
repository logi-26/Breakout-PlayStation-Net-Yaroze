#include "collision.h"
#include "game.h"

// Checks if two rectangles overlap
int RectanglesOverlap(GsBOXF* first, GsBOXF* second) {
    int xOverlap = XOverlap(first, second);
    int yOverlap = YOverlap(first, second);
    return xOverlap && yOverlap;
}

// Checks if two rectangles overlap on the x-axis
int XOverlap(GsBOXF* first, GsBOXF* second) {
    return (first->x < second->x + second->w) && 
           (first->x + first->w > second->x);
}

// Checks if two rectangles overlap on the y-axis
int YOverlap(GsBOXF* first, GsBOXF* second) {
    return (first->y < second->y + second->h) && 
           (first->y + first->h > second->y);
}

// Checks if two rectangles touch
int RectanglesTouch(GsBOXF* first, GsBOXF* second) {
    int xTouch = XTouch(first, second);
    int yTouch = YTouch(first, second);
    int xOverlap = XOverlap(first, second);
    int yOverlap = YOverlap(first, second);
    return (xTouch && yOverlap) || (yTouch && xOverlap);
}

// Checks if two rectangles touch on the x-axis
int XTouch(GsBOXF* first, GsBOXF* second) {
    return (first->x == second->x + second->w) || 
           (first->x + first->w == second->x);
}

// Checks if two rectangles touch on the y-axis
int YTouch(GsBOXF* first, GsBOXF* second) {
    return (first->y == second->y + second->h) || 
           (first->y + first->h == second->y);
}

// Determines which part touches on the x-axis
int GetXtouch(GsBOXF* first, GsBOXF* second) {
    if (first->x == second->x + second->w) return LEFT;
    if (first->x + first->w == second->x) return RIGHT;
    if (first->x < second->x + second->w && first->x + first->w > second->x) return MIDDLE;
    return NONE;
}

// Determines which part touches on the y-axis
int GetYtouch(GsBOXF* first, GsBOXF* second) {
    if (first->y == second->y + second->h) return TOP;
    if (first->y + first->h == second->y) return BOTTOM;
    if (first->y < second->y + second->h && first->y + first->h > second->y) return MIDDLE;
    return NONE;
}

// Determines where the rectangles touch
int FindWhereRectanglesTouch(GsBOXF* first, GsBOXF* second) {
    static const int touchTable[4][4] = {
        {TOP_LEFT,    TOP,       TOP_RIGHT,    NONE},
        {LEFT,        OVERLAP,   RIGHT,        NONE},
        {BOTTOM_LEFT, BOTTOM,    BOTTOM_RIGHT, NONE},
        {NONE,        NONE,      NONE,         NONE}
    };

    int xTouch = GetXtouch(first, second);
    int yTouch = GetYtouch(first, second);

    return touchTable[yTouch][xTouch];
}