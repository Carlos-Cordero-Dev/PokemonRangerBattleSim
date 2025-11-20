
#include "world_object.h"
#include "animation_database.h"
#include "timer.h"

WorldObject::WorldObject(const std::vector<SpriteAnimation*>& animations)
{
	//deep copy so updating an object doesnt cause the anim of other to update too

	this->animations.reserve(animations.size());
	for (auto* anim : animations)
		this->animations.push_back(new SpriteAnimation(*anim));
	//bad: this->animations = animations;

	position.x = 0;
	position.y = 0;

	scale = 1.0f;
	rotationDeg = 0.0f;

	boundingBox = Rectangle();
	boundingBox.x = position.x;
	boundingBox.y = position.y;

	//TODO: eventually this comes from metadata from each frame
	boundingBox.width = animations[0]->animData_->textures[0].texture.width;
	boundingBox.height = animations[0]->animData_->textures[0].texture.height;

}

void WorldObject::Draw()
{
	animations[0]->DrawRotScale(
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
	animations[0]->Update();

	UpdateBBoxPosition();
}

void WorldObject::UpdateBBoxPosition()
{
	boundingBox.x = position.x;
	boundingBox.y = position.y;
}

void WorldObject::Cleanup()
{
	//dont clear sprite anim cause the underlying pointer is cleared in main (hopefully)
}