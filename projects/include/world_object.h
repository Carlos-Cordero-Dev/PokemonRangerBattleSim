
#pragma once

#include <raylib.h>

#include "sprites.h"

//TODO: propper state machine
enum StateMachine
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
	StateMachine state;

	Rectangle boundingBox;
	std::vector<SpriteAnimation*> animations;
};

 