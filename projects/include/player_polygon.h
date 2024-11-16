
#ifndef __PLAYER_POLYGON_H__
#define __PLAYER_POLYGON_H__

#include "FIFO.h"

void DrawCircle(int radius,int x, int y);

void DrawCurrentPolygon(Coord *stack);

bool IsPolygonClosed(Coord *stack, int x,int y);

#endif
