
#include "visual_effect.h"

#include "pokemon.h"
#include "world_object.h"
#include "animation_database.h"
#include "sprites.h"


VisualEffect::VisualEffect(
	AnimationData* animationData,
	WorldObject* owner,
	const std::string& animationName,
	Vector2 offset,
	float scale,
	float rotationDeg,
	bool flipX,
	bool followOwner,
	bool drawBehindOwner)
	: animation(animationData),
	owner(owner),
	animationName(animationName),
	offset(offset),
	scale(scale),
	rotationDeg(rotationDeg),
	flipX(flipX),
	followOwner(followOwner),
	drawBehindOwner(drawBehindOwner)
{
	animation.SetLoop(true);
	animation.SetEventsEnabled(false);
	if (owner) {
		position = {
			owner->position.x + offset.x,
			owner->position.y + offset.y
		};
	}
}

void VisualEffect::Update()
{
	if (followOwner && owner) {
		const AnimationVariant* variant = static_cast<Pokemon*>(owner)->ResolveAnimation(animationName);
		if (variant && variant->data) {
			animation.SetAnimationData(variant->data, true);
			flipX = variant->flipX;
		}
		position = {
			owner->position.x + offset.x,
			owner->position.y + offset.y
		};
	}
	animation.Update(position.x, position.y);
}

void VisualEffect::Draw()
{
	animation.DrawRotScaleCentered(
		position.x,
		position.y,
		rotationDeg,
		scale,
		flipX);
}