
#pragma once

#include "raylib.h"
#include "raymath.h"

#include "coord.h"

#include <algorithm>
#include <vector>

#include "collision.h"
#include "world_object.h"


//#include "enclosable_object.h"
class EnclosableObject;
class SpriteAnimation;

class Top
{
public:
	Top();
	void Update();
	void Draw();

	WorldObject* wo = nullptr;
	SpriteAnimation* trail_start_anim = nullptr;
	SpriteAnimation* trail_end_anim = nullptr;
	float tail_anim_scale = 1.0f;

	static constexpr Vector2 kFromTouchOffset = { 0.0f, -27.0f };

	Coord* stack = nullptr;
	float distance = 0.0f;
	
	bool wasDamaged = false; // true when last update top hit a hibox/world object , resets on touch up

};

void InsertTopCoord(Top* top, int x, int y);


float ComputeAndUpdateDistance(Top* top);

void ForceTopDistanceLimit(Top* top);

void ResetTop(Top* top);

//======= intersection stuff  =============================================

const int kMinDepth = 10; // min number of nodes to consider a closed polygon
const double kMinArea = 500; //min area to consider close polygon

Coord* DeepcopyPolyStartEnd(Coord* startNode, Coord* endNode);

// in: every enclosable object to check against
// out: enclosed convex polygon that forms from the intersection, point where last encosed object was (TODO: idk what happens when multiple objects are enclosed)
// return true when enclosed any object, false otherwise
bool checkTopIntersection(Top* top, const std::vector<EnclosableObject*>& inEnclosableObjs, Coord*& outEnclosedPolygon, Vector2* outEnclosedCenter);

inline float Dist(Coord* c1, Coord* c2)
{
	Vector2 v1 = { c1->x,c1->y };
	Vector2 v2 = { c2->x,c2->y };

	float dist = Vector2Distance(v1, v2);
	return dist;
}