
#include "hitbox.h"

#include <stdio.h>

Hitbox::Hitbox(float centerX, float centerY, float width, float height)
{
	boundingBox.x = centerX - width /2.0f;
	boundingBox.y = centerY - height/2.0f;
	boundingBox.width = width;
	boundingBox.height = height;

}

void Hitbox::OnCollision()
{
	printf("hitbox collided");
}

void Hitbox::ShowHitbox()
{
	DrawRectangleLines(
		boundingBox.x,
		boundingBox.y,
		boundingBox.width,
		boundingBox.height,
		RED
	);
}