
#pragma once

/*

-doesnt retract with time
-only retracts with distance
-retracts distance each frame, doesnt retract x units per frame, but updates in the current frame:
	if you moved and now the you are at 91 length and the max is 80, in that same frame it will go to 80 forcefully
	if you were at 82 it will only move 2 , so it isnt fixed rate
-for the above to work you have to have a history every single point youve been in so update of points should be instant
-closing a circle is done when intersection with your own line (dot product > x maybe)
-really low threshold to consider a circle done you can almost do circles in a 5x5 pixels
-on circle done not reset but top is placed on intersection and updates length
-touching pokemon or recieving atack completely deletes top
-having top destroyed forces to click again 

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
		printf("skiped exsisting %d %d\n", x, y);
		return;
	}
	InsertCoord(&top->stack, x, y);
	if (top->stack->nextCoord != nullptr)
	{
		Coord* next = top->stack->nextCoord;
		float dist =  Vector2Distance({ (float)x,(float)y }, { (float)next->x,(float)next->y });
		top->distance += dist;
		top->stack->distance_at_point = top->distance;
		printf("new dist %f\n", top->distance);
	}
}
constexpr float kMaxDistance = 700.0f;
//constexpr float kMinStep = 1.0f;
//constexpr float kMaxStep = 15.0f;

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


		Vector2 lastVec = { (float)lastCoord->x,(float)lastCoord->y };
		Vector2 lastMinusOneVec = { (float)lastMinusOneCoord->x,(float)lastMinusOneCoord->y };

		float dist = Vector2Distance(lastMinusOneVec,lastVec);

		//nope for now
		//if (dist > kMaxStep)
		//{
		//	dist = kMaxStep;

		//	//calculate position of lastMinusOneCoord for the top to correctly jump to that spot at kMaxStep distnace from lastCoord
		//	Vector2 dir = Vector2Normalize({ lastMinusOneVec.x - lastVec.x , lastMinusOneVec.y - lastVec.y } );
		//	Vector2 newLastMinusOneLocation =  Vector2Add(lastVec, Vector2Scale(dir,kMaxStep));

		//	lastMinusOneCoord->x = newLastMinusOneLocation.x;
		//	lastMinusOneCoord->y = newLastMinusOneLocation.y;
		//}

		top->distance -= dist;

		//printf("deleted %f curr dist %f\n", dist, top->distance);

		if (lastCoord != nullptr)
		{
			lastMinusOneCoord->nextCoord = nullptr;
			free(lastCoord); lastCoord = nullptr;
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

// Main function to check for intersection
const int kMinDepth = 10;
const int kMinDistance = 80;

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

			head->intersected = true;
			headNext->intersected = true;
			current->intersected = true;
			current->nextCoord->intersected = true;
			//printf("intersected %d %d %d %d distance %d point2point \n", head->depth, headNext->depth, 
			//	current->depth, current->nextCoord->depth, distance);
			return true;
		}
		current = current->nextCoord;
	}
	return false;
}
