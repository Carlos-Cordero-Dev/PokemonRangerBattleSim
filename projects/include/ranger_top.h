
#pragma once

/*
[TODO]-update code so it doesnt change line direction when intersecting (maybe add a node)
-doesnt retract with time
-only retracts with distance
-retracts distance each frame, doesnt retract x units per frame, but updates in the current frame:
	if you moved and now the you are at 91 length and the max is 80, in that same frame it will go to 80 forcefully
	if you were at 82 it will only move 2 , so it isnt fixed rate
-for the above to work you have to have a history every single point youve been in so update of points should be instant
[x]-closing a circle is done when intersection with your own line (dot product > x maybe)
-really low threshold to consider a circle done you can almost do circles in a 5x5 pixels
[x]-on circle done not reset but top is placed on intersection 
	and updates length
-touching pokemon or recieving atack completely deletes top
-having top destroyed forces to click again 

[x]-get intersection point
-figure out sound
*/

#include "FIFO.h"

#include "raymath.h"

#include <algorithm>


struct Top
{
	Coord* stack = nullptr;
	float distance = 0.0f;
};

void InsertTopCoord(Top* top, int x, int y)
{
	//dont insert duplicated coords if they user is holding stylus in place
	if (top->stack && (top->stack->x == x && top->stack->y == y))
	{
		//printf("skiped exsisting %d %d\n", x, y);
		return;
	}

	InsertCoord(&top->stack, x, y);
}

inline float Dist(Coord* c1, Coord* c2)
{
	Vector2 v1 = { (float)c1->x,(float)c1->y };
	Vector2 v2 = { (float)c2->x,(float)c2->y };

	float dist = Vector2Distance(v1, v2);
	return dist;
}

constexpr float kMaxDistance = 700.0f;
//constexpr float kMinStep = 1.0f;
//constexpr float kMaxStep = 15.0f;

float ComputeAndUpdateDistance(Top* top)
{
	if (top->stack == nullptr || top->stack->nextCoord == nullptr) {
		//printf("Insufficient coordinates in stack.\n");
		return 0.0f;
	}

	float totalDistance = 0.0f;
	top->stack->distance_at_point = 0;
	for (Coord* aux = top->stack; aux->nextCoord != nullptr; aux = aux->nextCoord)
	{
		float dist = Dist(aux, aux->nextCoord);
		aux->nextCoord->distance_at_point = dist + aux->distance_at_point;
		totalDistance += dist;
		//printf(" %f ", aux->distance_at_point);
	}

	top->distance = totalDistance;
	//printf("total distance %f\n", totalDistance);
	return totalDistance;
}

void ForceTopDistanceLimit(Top* top)
{
	while (top->distance > kMaxDistance)
	{
		if (top->stack == nullptr || top->stack->nextCoord == nullptr) {
			//printf("Insufficient coordinates in stack.\n");
			return;
		}
		Coord* lastMinusOneCoord = BotStack(top->stack, -1);
		Coord* lastCoord = BotStack(top->stack,0);

		if (lastCoord == nullptr || lastMinusOneCoord == nullptr)
		{
			//printf("Null pointer encountered in stack.\n");
			return;
		}

		float distBetweenLastAndLastMinusOne = lastCoord->distance_at_point - lastMinusOneCoord->distance_at_point;

		//if last minus one is still too far destroy last and try again
		if (lastMinusOneCoord->distance_at_point > kMaxDistance)
		{
			lastMinusOneCoord->nextCoord = nullptr;
			free(lastCoord); lastCoord = nullptr;
			top->distance -= distBetweenLastAndLastMinusOne;
			continue;
			//printf("too far\n");
		}

		//force to kMaxdistance if the step is too big by moving lastCoord to the kMaxDistance point
		if (lastMinusOneCoord->distance_at_point < kMaxDistance)
		{
			Vector2 lastCoordVec = { (float)lastCoord->x,(float)lastCoord->y };
			Vector2 lastMinusOneCoordVec = { (float)lastMinusOneCoord->x,(float)lastMinusOneCoord->y };
			Vector2 lastMinusOneToLastVec = Vector2Subtract(lastCoordVec, lastMinusOneCoordVec);

			float distLeftToCover = kMaxDistance - lastMinusOneCoord->distance_at_point;
			Vector2 vecToMaxDist = Vector2Scale(Vector2Normalize(lastMinusOneToLastVec), distLeftToCover);

			Vector2 newLastPosVec =  Vector2Add(lastMinusOneCoordVec, vecToMaxDist);

			lastCoord->x = newLastPosVec.x;
			lastCoord->y = newLastPosVec.y;
			lastCoord->distance_at_point = kMaxDistance;
			top->distance = kMaxDistance;
			//printf("near\n");
		}
	}

}

void ResetTop(Top* top)
{
	DestroyStack(&top->stack); top->stack = nullptr;
	top->distance = 0;
}

//======= intersection stuff  =============================================

// Check if point r is on segment pq
bool onSegment(Coord* p, Coord* q, Coord* r) {
	return r->x <= std::max(p->x, q->x) && r->x >= std::min(p->x, q->x) &&
		r->y <= std::max(p->y, q->y) && r->y >= std::min(p->y, q->y);
}

// Determine the orientation of the triplet (p, q, r)
// 0 -> collinear, 1 -> clockwise, 2 -> counterclockwise
int orientation(Coord* p, Coord* q, Coord* r) {
	int val = (q->y - p->y) * (r->x - q->x) -
		(q->x - p->x) * (r->y - q->y);
	if (val == 0) return 0;           // Collinear
	return (val > 0) ? 1 : 2;         // Clockwise or Counterclockwise
}

// Check if two segments p1q1 and p2q2 intersect
bool doIntersect(Coord* p1, Coord* q1, Coord* p2, Coord* q2) {
	// Find the orientations
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case
	if (o1 != o2 && o3 != o4) return true;

	// Special Cases
	if (o1 == 0 && onSegment(p1, q1, p2)) return true;
	if (o2 == 0 && onSegment(p1, q1, q2)) return true;
	if (o3 == 0 && onSegment(p2, q2, p1)) return true;
	if (o4 == 0 && onSegment(p2, q2, q1)) return true;

	return false; // Doesn't fall in any of the cases
}

bool getIntersectionPoint(Coord* p1, Coord* q1, Coord* p2, Coord* q2, Coord& intersection) {
	// Line equations: p1 + t1 * (q1 - p1) = p2 + t2 * (q2 - p2)
	int a1 = q1->y - p1->y;
	int b1 = p1->x - q1->x;
	int c1 = a1 * p1->x + b1 * p1->y;

	int a2 = q2->y - p2->y;
	int b2 = p2->x - q2->x;
	int c2 = a2 * p2->x + b2 * p2->y;

	int determinant = a1 * b2 - a2 * b1;

	if (determinant == 0) return false; // Parallel lines

	// Calculate intersection point
	intersection.x = (b2 * c1 - b1 * c2) / determinant;
	intersection.y = (a1 * c2 - a2 * c1) / determinant;
	return true;
}

// Shoelace formula to calculate the area of a polygon
double calculatePolygonArea(Coord* start, Coord* end) {
	if (!start || !end) return 0.0;

	double area = 0.0;
	Coord* current = start;
	Coord* next = nullptr;

	// Traverse from start to end (inclusive) and apply the Shoelace formula
	while (current != end) {
		next = current->nextCoord;
		if (!next) break;

		area += current->x * next->y - current->y * next->x;
		current = next;
	}

	// Add the last segment (end to start) to close the polygon
	area += current->x * start->y - current->y * start->x;

	return fabs(area) / 2.0;
}

const int kMinDepth = 10;
const double kMinArea = 1000;

bool checkSnakeIntersection(Top* top) {
	Coord* head = top->stack;
	if (head == nullptr) return false;
	if (head->nextCoord == nullptr) return false;

	Coord* headNext = head->nextCoord;

	// starts at the kMinDepth point cause you can intersect with your adjacent segment
	Coord* current = headNext;
	while (current && current->nextCoord) {
		if (doIntersect(head, headNext, current, current->nextCoord)) {

			//printf("tried intersect %d %d %d %d\n", head->depth, headNext->depth, current->depth, current->nextCoord->depth);
			int depth = headNext->depth - current->depth;
			//TODO: distance hard to implement bc you are deleting nodes buddy
			//int distance = head->distance_at_point - current->distance_at_point;
			if ((depth < kMinDepth) /*|| (distance < kMinDistance)*/)
			{
				current = current->nextCoord;
				continue;
			}

			//check if area is big enough
			Coord intersection;
			if (!getIntersectionPoint(head, headNext, current, current->nextCoord,intersection))
			{
				current = current->nextCoord;
				continue;
			}
			int oldHeadX = head->x;
			int oldHeadY = head->y;
			int oldcurrentX = current->x;
			int oldcurrentY = current->y;

			head->x = intersection.x;
			head->y = intersection.y;
			current->x = intersection.x;
			current->y = intersection.y;

			double area = calculatePolygonArea(head, current);

			head->x = oldHeadX;
			head->y = oldHeadY;
			current->x = oldcurrentX;
			current->y = oldcurrentY;

			if (area < kMinArea)
			{
				current = current->nextCoord;
				continue;
			}

			head->intersected = true;
			headNext->intersected = true;
			current->intersected = true;
			current->nextCoord->intersected = true;
			printf("intersected %d %d %d %d  point %d %d area %f\n", head->depth, headNext->depth,
				current->depth, current->nextCoord->depth, intersection.x, intersection.y, area);

			//printf("depth headnext %d current %d \n", headNext->depth, current->depth);
			freeCoordsBackward(headNext,current);

			current->x = intersection.x;
			current->y = intersection.y;

			head->nextCoord = current;

			return true;
		}
		current = current->nextCoord;
	}
	return false;
}
