
#pragma once

#include <raylib.h>

#include "sprites.h"

class WorldObject
{
public:

	WorldObject(SpriteAnimation** sa);

	void Draw(int frame); //draw sprite at position, defaults at anim 0
	void Draw(int frame,int animation);
	void Update();
	void Cleanup();

	void OnCollision() { printf("collided"); };

public:

	Vector2 position;
	float scale;
	float rotationDeg;
	bool canCollide = true;

	Rectangle boundingBox;
	SpriteAnimation** spriteAnim;
};

 