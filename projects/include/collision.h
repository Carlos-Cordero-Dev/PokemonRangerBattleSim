
#pragma once

#include "FIFO.h"

/* PENDING

-collision of all list at all points with a collision box (optimize it, cache it prolly)
-not relevant here but pretty yellow fade effect.

*/

struct Point 
{
	float x, y;
};

bool IsIntersecting(Point a, Point b, Point c, Point d);

bool PolygonHeadCollidingWithLine(Coord* stack, int lineCount /*starting from head*/, Point lineOrgin, Point lineEnd);

//this collision includes checks for middle of the box with 2 lines from opposite vertexes
bool PolygonHeadCollidingWithBox(Coord* stack, int lineCount, float boxOriginX, float boxOriginY, float boxWidth, float boxHeight);

bool IsBoxInsideOfPoly(float boxOriginX, float boxOriginY, float boxWidth, float boxHeight, Point* polygonPoints, int numberOfPolygonPoints);

int pnpoly(Point P, Point* V, int polyNodeCount);

Point* CoordListToPointList(Coord* poly_start, Coord* poly_end);
