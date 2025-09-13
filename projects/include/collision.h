
#pragma once

#include "FIFO.h"
#include "raylib.h"

/* PENDING

-not relevant here but pretty yellow fade effect.
-consider using broad-narrow phase collision??? prolly not worth it for our impl
*/

struct Point 
{
	float x, y;
};

bool inline IsIntersecting(Point a, Point b, Point c, Point d);

bool PolygonCollidingWithLine(Coord* stack, Point lineOrgin, Point lineEnd);

bool PolygonCollidingWithBox(Coord* stack, float boxOriginX, float boxOriginY, float boxWidth, float boxHeight);
bool PolygonCollidingWithBox(Coord* stack,Rectangle bb);

bool PolygonHeadCollidingWithLine(Coord* stack, int lineCount /*starting from head*/, Point lineOrgin, Point lineEnd);

//this collision includes checks for middle of the box with 2 lines from opposite vertexes
bool PolygonHeadCollidingWithBox(Coord* stack, int lineCount, float boxOriginX, float boxOriginY, float boxWidth, float boxHeight);

bool IsBoxInsideOfPoly(float boxOriginX, float boxOriginY, float boxWidth, float boxHeight, Point* polygonPoints, int numberOfPolygonPoints);

int pnpoly(Point P, Point* V, int polyNodeCount);

Point* CoordListToPointList(Coord* poly_start, Coord* poly_end);
