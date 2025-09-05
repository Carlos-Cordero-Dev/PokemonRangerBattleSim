
#include "world_object.h"

WorldObject::WorldObject(SpriteAnimation** sa)
{
	spriteAnim = sa;

	position.x = 0;
	position.y = 0;

	scale = 1.0f;
	rotationDeg = 0.0f;

	boundingBox = Rectangle();
	boundingBox.x = position.x;
	boundingBox.y = position.y;
	boundingBox.width = sa[0]->textures[0].texture.width;
	boundingBox.height = sa[0]->textures[0].texture.height;

}

void WorldObject::Draw(int frame)
{
	spriteAnim[0]->DrawRotScale(
		position.x - boundingBox.width * scale /2,
		position.y - boundingBox.height * scale / 2,
		rotationDeg, scale);
	
	spriteAnim[0]->advanceFrame(frame);

	//debug draw bounding box
	DrawRectangleLines(
		boundingBox.x - boundingBox.width * scale / 2 ,
		boundingBox.y - boundingBox.height * scale / 2,
		boundingBox.width * scale,boundingBox.height * scale,
		RED
	);
}

void WorldObject::Draw(int frame, int animation)
{
	spriteAnim[animation]->DrawRotScale(
		position.x - boundingBox.width * scale / 2,
		position.y - boundingBox.height * scale / 2,
		rotationDeg, scale);

	spriteAnim[animation]->advanceFrame(frame);

	//debug draw bounding box
	DrawRectangleLines(
		boundingBox.x - boundingBox.width * scale / 2,
		boundingBox.y - boundingBox.height * scale / 2,
		boundingBox.width * scale, boundingBox.height * scale,
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