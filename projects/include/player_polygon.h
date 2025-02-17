
#ifndef __PLAYER_POLYGON_H__
#define __PLAYER_POLYGON_H__

#include "FIFO.h"
#include "constants.h"

void DrawCircle(int radius,int x, int y);

void DrawCurrentPolygon(Coord *stack);

void DrawCurrentPolygonOnlyLines(Coord* stack);

bool IsPolygonClosed(Coord *stack, int x,int y);

void ComputeRectangle(float start[2], float end[2], float topLeft[2], float topRight[2], float botLeft[2], float botRight[2]);

struct TopPointData {
	int start[2];
	int end[2];
	int topLeft[2];
	int topRight[2];
	int botLeft[2];
	int botRight[2];
};

void PopulateTopPoints(TopPointData* topPointData, int numberOfPoints);

void ClearPointData(TopPointData* topPointData, int numberOfPoints);

#endif
