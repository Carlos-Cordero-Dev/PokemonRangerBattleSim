
#include "world_object.h"
#include "animation_database.h"
#include "timer.h"

WorldObject::WorldObject(SpriteAnimation* animation)
{
	//deep copy so updating an object doesnt cause the anim of other to update too

	this->animations.reserve(1);
	this->animations.push_back(new SpriteAnimation(*animation));
	//bad: this->animations = animations;

	position.x = 0;
	position.y = 0;

	scale = 1.0f;
	rotationDeg = 0.0f;

	boundingBox = Rectangle();
	boundingBox.x = position.x;
	boundingBox.y = position.y;

	//TODO: eventually this comes from metadata from each frame
	if (!animations.empty() && animations[0] && animations[0]->animData_ &&
		!animations[0]->animData_->textures.empty()) {
		baseWidth = animations[0]->animData_->textures[0].texture.width;
		boundingBox.width = baseWidth;
		baseHeight = animations[0]->animData_->textures[0].texture.height;
		boundingBox.height = baseHeight;
	}
}

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
	if (!animations.empty() && animations[0] && animations[0]->animData_ &&
		!animations[0]->animData_->textures.empty()) {
		baseWidth = animations[0]->animData_->textures[0].texture.width;
		boundingBox.width = baseWidth;
		baseHeight = animations[0]->animData_->textures[0].texture.height;
		boundingBox.height = baseHeight;
	}

}

void WorldObject::Draw()
{
	//TODO: technically this boundbox w/h should be the w/h of the sprite, not the bb
	animations[0]->DrawRotScaleCentered(
		position.x,
		position.y,
		rotationDeg, scale);
	
	//debug draw bounding box
	DrawRectangleLines(
		boundingBox.x,
		boundingBox.y,
		boundingBox.width,
		boundingBox.height,
		RED
	);
	//DrawRectangleLinesEx(boundingBox,1.0f,RED);

	//debug center position
	DrawCircleV(position, 5.5f, GREEN);
}

void WorldObject::Update()
{
	animations[0]->Update(position.x,position.y);

	UpdateBBox();
}

void WorldObject::UpdateBBox()
{
	boundingBox.width = baseWidth * scale;
	boundingBox.height = baseHeight * scale;

	//TODO: center everything bc theres a missmatch between the bb thats top left aligned and some stuff thats "center" aligned -w,-h etc
	boundingBox.x = position.x - boundingBox.width/ 2;
	boundingBox.y = position.y - boundingBox.height/ 2;
}

void WorldObject::Cleanup()
{
	//dont clear sprite anim cause the underlying pointer is cleared in main (hopefully)
}