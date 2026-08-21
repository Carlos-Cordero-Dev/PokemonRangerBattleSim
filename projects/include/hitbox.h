
#pragma once

#include <raylib.h>

class Hitbox {

public:
	Hitbox(float centerX, float centerY, float width, float height, Vector2 velocity, float delaySec);

	void Update(float dt);
	void OnCollision();

	//debug draw
	void ShowHitbox();

	Rectangle boundingBox;
	Vector2 velocity;
	float spawnDelaySec = 0.0f;
	float spawnDelayElapsedSec = 0.0f;
	bool isActive = false;
};