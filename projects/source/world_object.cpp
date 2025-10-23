
#include "world_object.h"
#include "animation_database.h"
#include "timer.h"


WorldObject::WorldObject(const std::vector<SpriteAnimation*>& animations)
{
	this->animations = animations;
	this->animState = AnimationState::kStateIdle;

	position.x = 0;
	position.y = 0;

	scale = 1.0f;
	rotationDeg = 0.0f;

	boundingBox = Rectangle();
	boundingBox.x = position.x;
	boundingBox.y = position.y;

	//TODO: eventually this comes from metadata from each frame
	boundingBox.width = animations[0]->animData->textures[0].texture.width;
	boundingBox.height = animations[0]->animData->textures[0].texture.height;

}

void WorldObject::Draw()
{
	animations[animState]->DrawRotScale(
		position.x - boundingBox.width * scale /2,
		position.y - boundingBox.height * scale / 2,
		rotationDeg, scale);
	
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
	animations[animState]->Update();

	boundingBox.x = position.x;
	boundingBox.y = position.y;
}

void WorldObject::Cleanup()
{
	//dont clear sprite anim cause the underlying pointer is cleared in main (hopefully)
}