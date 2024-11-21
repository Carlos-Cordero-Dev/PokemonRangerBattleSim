
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

*/

#include "FIFO.h"
#include "raymath.h"

struct Top
{
	Coord* stack = nullptr;
	float distance = 0.0f;
};

void InsertTopCoord(Top* top, int x, int y)
{
	InsertCoord(&top->stack, x, y);
	if (top->stack->nextCoord != nullptr)
	{
		Coord* next = top->stack->nextCoord;
		float dist =  Vector2Distance({ (float)x,(float)y }, { (float)next->x,(float)next->y });
		top->distance += dist;
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
			printf("Insufficient coordinates in stack.\n");
			return;
		}
		Coord* lastMinusOneCoord = BotStack(top->stack, -1);
		Coord* lastCoord = BotStack(top->stack,0);

		if (lastCoord == nullptr || lastMinusOneCoord == nullptr)
		{
			printf("Null pointer encountered in stack.\n");
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

		printf("deleted %f curr dist %f\n", dist, top->distance);

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