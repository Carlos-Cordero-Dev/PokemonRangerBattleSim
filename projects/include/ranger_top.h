
#pragma once

/*
[X]-update code so it doesnt change line direction when intersecting (maybe add a node)
[x]-doesnt retract with time
[x]-only retracts with distance
[x]-retracts distance each frame, doesnt retract x units per frame, but updates in the current frame:
	if you moved and now the you are at 91 length and the max is 80, in that same frame it will go to 80 forcefully
	if you were at 82 it will only move 2 , so it isnt fixed rate
[x]-for the above to work you have to have a history every single point youve been in so update of points should be instant
[x]-closing a circle is done when intersection with your own line (dot product > x maybe)
-really low threshold to consider a circle done you can almost do circles in a 5x5 pixels
[x]-on circle done not reset but top is placed on intersection 
[x]	and updates length
[x]-get intersection point

-touching pokemon or recieving atack completely deletes top
-having top destroyed forces to click again 
-figure out sound

*/

#include "raylib.h"
#include "raymath.h"

#include "FIFO.h"

#include <algorithm>
#include <vector>

#include "enclosable_object.h"
#include "collision.h"

struct Top
{
	Coord* stack = nullptr;
	float distance = 0.0f;
};

void InsertTopCoord(Top* top, int x, int y);

inline float Dist(Coord* c1, Coord* c2)
{
	Vector2 v1 = { c1->x,c1->y };
	Vector2 v2 = { c2->x,c2->y };

	float dist = Vector2Distance(v1, v2);
	return dist;
}

float ComputeAndUpdateDistance(Top* top);

void ForceTopDistanceLimit(Top* top);

void ResetTop(Top* top);

//======= intersection stuff  =============================================

// Check if point r is on segment pq
bool inline onSegment(Coord* p, Coord* q, Coord* r);

// Determine the orientation of the triplet (p, q, r)
// 0 -> collinear, 1 -> clockwise, 2 -> counterclockwise
int inline orientation(Coord* p, Coord* q, Coord* r);

// Check if two segments p1q1 and p2q2 intersect
bool inline doIntersect(Coord* p1, Coord* q1, Coord* p2, Coord* q2);

bool inline getIntersectionPoint(Coord* p1, Coord* q1, Coord* p2, Coord* q2, Coord& intersection);

// Shoelace formula to calculate the area of a polygon
double calculatePolygonArea(Coord* start, Coord* end);

const int kMinDepth = 10; // min number of nodes to consider a closed polygon
const double kMinArea = 1000; //min area to consider close polygon

Coord* DeepcopyPolyStartEnd(Coord* startNode, Coord* endNode);

//returns the enclosed convex polygon that forms from the intersection

Coord* checkTopIntersection(Top* top, const std::vector<EnclosableObject*>& enclosableObjs);

void CalculateEnclosedShaderAreaPoints(Coord* enclosedPoints, std::vector<Vector2>& vectorToFill);

void CalculateClosingIndicatorParticlePoints(Coord* enclosedPoints, std::vector<Vector2>& outParticlePositions);
