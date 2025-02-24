#ifndef COLLISION_H
#define COLLISION_H

#include "game.h"

int RectanglesOverlap(GsBOXF* first, GsBOXF* second);
int XOverlap(GsBOXF* first, GsBOXF* second);
int YOverlap(GsBOXF* first, GsBOXF* second);
int RectanglesTouch(GsBOXF* first, GsBOXF* second);
int XTouch(GsBOXF* first, GsBOXF* second);
int YTouch(GsBOXF* first, GsBOXF* second);
int FindWhereRectanglesTouch(GsBOXF* first, GsBOXF* second);

#endif