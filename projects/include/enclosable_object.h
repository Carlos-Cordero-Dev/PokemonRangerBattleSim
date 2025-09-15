
#pragma once

#include "world_object.h"
#include "sprites.h"

class EnclosableObject : public WorldObject
{
public:
	EnclosableObject(const std::vector<SpriteAnimation*>& animations);

	virtual void OnEnclosed() = 0;

	virtual ~EnclosableObject() = default;
};