
#pragma once

#include <raylib.h>

class Hitbox {

public:
	Hitbox(float centerX, float centerY, float width, float height);

	void OnCollision();

	//debug draw
	void ShowHitbox();

	Rectangle boundingBox;
};