
#include "world_object.h"

WorldObject::WorldObject(SpriteAnimation* sa)
{
	spriteAnim = sa;

	position.x = 0;
	position.y = 0;

	scale = 1.0f;
	rotationDeg = 0.0f;

	boundingBox = Rectangle();
	boundingBox.x = position.x;
	boundingBox.y = position.y;
	boundingBox.width = sa->textures[0].texture.width;
	boundingBox.height = sa->textures[0].texture.height;

}

void WorldObject::Draw(int frame)
{
	spriteAnim->DrawRotScale(
		position.x - boundingBox.width * scale /2,
		position.y - boundingBox.height * scale / 2,
		rotationDeg, scale);
	
	spriteAnim->advanceFrame(frame);

	//debug draw bounding box
	DrawRectangleLines(
		boundingBox.x - boundingBox.width * scale / 2 ,
		boundingBox.y - boundingBox.height * scale / 2,
		boundingBox.width * scale,boundingBox.height * scale,
		RED
	);
}

void WorldObject::Update()
{
	boundingBox.x = position.x;
	boundingBox.y = position.y;
}

void WorldObject::Cleanup()
{
	//dont clear sprite anim cause the underlying pointer is cleared in main (hopefully)
}