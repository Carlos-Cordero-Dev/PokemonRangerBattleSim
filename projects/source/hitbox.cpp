
#include "hitbox.h"

#include <stdio.h>

Hitbox::Hitbox(float centerX, float centerY, float width, float height,Vector2 velocity = {0.0f,0.0f}, float delaySec = 0.0f)
{
	boundingBox.x = centerX - width /2.0f;
	boundingBox.y = centerY - height/2.0f;
	boundingBox.width = width;
	boundingBox.height = height;

	this->velocity = velocity;
	spawnDelaySec = delaySec;
	spawnDelayElapsedSec = 0.0f;

	// false if delay exists, true otherwise
	isActive = (delaySec <= 0.0f);
}

void Hitbox::Update(float dt)
{
	if (!isActive)
	{
		spawnDelayElapsedSec += dt;
		if(spawnDelayElapsedSec >= spawnDelaySec)
		{
			isActive = true;
		}
	}
	else //isActive
	{
		boundingBox.x += velocity.x * dt;
		boundingBox.y += velocity.y * dt;
	}
}

void Hitbox::OnCollision()
{
	printf("hitbox collided");
}


void Hitbox::ShowHitbox()
{
	if (isActive)
	{
		DrawRectangleLines(
			boundingBox.x,
			boundingBox.y,
			boundingBox.width,
			boundingBox.height,
			RED
		);
	}
	else //!isActive
	{
		DrawRectangleLines(
			boundingBox.x,
			boundingBox.y,
			boundingBox.width,
			boundingBox.height,
			YELLOW
		);
	}
}