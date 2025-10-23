
#pragma once

#include <raylib.h>

#include "sprites.h"
#include "state_machine.h"

enum AnimationState
{
	kStateIdle = 0,
	kMAXState
};

class WorldObject
{
public:

	WorldObject(const std::vector<SpriteAnimation*>& animations);

	void Draw();
	void Update();
	void Cleanup();

	void OnCollision() { /*printf("collided");*/ };

public:

	Vector2 position;
	float scale;
	float rotationDeg;
	bool canCollide = true;
	
	AnimationState animState;

	Rectangle boundingBox;
	std::vector<SpriteAnimation*> animations;
};

 