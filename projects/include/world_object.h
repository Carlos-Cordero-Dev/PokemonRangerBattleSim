
#pragma once

#include <raylib.h>

#include "sprites.h"

class WorldObject
{
public:

	WorldObject(SpriteAnimation* sa);

	void Draw(int frame); //draw sprite at position
	void Update();
	void Cleanup();

	public:

	Vector2 position;
	float scale;
	float rotationDeg;

	Rectangle boundingBox;
	SpriteAnimation *spriteAnim;
};

 