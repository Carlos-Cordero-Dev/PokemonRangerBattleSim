
#pragma once

#include <raylib.h>

#include "sprites.h"
//#include "state_machine.h"

class WorldObject
{
public:

	WorldObject(const std::vector<SpriteAnimation*>& animations);
	virtual ~WorldObject() = default;

	virtual void Draw();
	virtual void Update();
	virtual void Cleanup();

	virtual void OnCollision() { printf("worldobj collided"); };

	void UpdateBBoxPosition();
public:

	//NOTE: position is always centered, boundinBox is topleft aligned and gets h,w/2 subtracted every time it needs to get drawn
	Vector2 position;
	float scale;
	float rotationDeg;
	bool canCollide = true;
	
	Rectangle boundingBox;

	// vector in case children want to have multiple animations, but worldObjects can only have one sprite so to 
	// not mess with the base impl of worldobject, so only objects with no state can exist in worldobject form.
	// see pokemon for an example where a state machine and different animationStates go thru the animations vector
	std::vector<SpriteAnimation*> animations;
};

 