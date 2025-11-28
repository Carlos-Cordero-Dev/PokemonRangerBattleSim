
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
	//TODO: technically this boundbox w/h should be the w/h of the sprite, not the bb
	animations[0]->DrawRotScale(
		position.x - boundingBox.width * scale / 2,
		position.y - boundingBox.height * scale / 2,
		rotationDeg, scale);
	
	//debug draw bounding box
	DrawRectangleLines(
		boundingBox.x,
		boundingBox.y,
		boundingBox.width * scale,
		boundingBox.height * scale,
		RED
	);
	//DrawRectangleLinesEx(boundingBox,1.0f,RED);

	//debug center position
	DrawCircleV(position, 5.5f, GREEN);
}

void WorldObject::Update()
{
	animations[0]->Update(position.x,position.y);

	UpdateBBoxPosition();
}

void WorldObject::UpdateBBoxPosition()
{
	//TODO: center everything bc theres a missmatch between the bb thats top left aligned and some stuff thats "center" aligned -w,-h etc
	boundingBox.x = position.x - boundingBox.width * scale/ 2;
	boundingBox.y = position.y - boundingBox.height * scale / 2;
}

void WorldObject::Cleanup()
{
	//dont clear sprite anim cause the underlying pointer is cleared in main (hopefully)
}