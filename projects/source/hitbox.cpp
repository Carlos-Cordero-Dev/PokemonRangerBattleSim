
#include "hitbox.h"

#include <stdio.h>

Hitbox::Hitbox(float centerX, float centerY, float width, float height)
{
	boundingBox.x = centerX;
	boundingBox.y = centerY;
	boundingBox.width = width;
	boundingBox.height = height;
}

void Hitbox::OnCollision()
{
	printf("hitbox collided\n");
}

void Hitbox::ShowHitbox()
{
	DrawRectangleLines(
		boundingBox.x - boundingBox.width / 2,
		boundingBox.y - boundingBox.height / 2,
		boundingBox.width, boundingBox.height,
		RED
	);
}