
#pragma once

#include "world_object.h"
#include "sprites.h"
#include "ranger_top.h"
#include "state_machine.h"

class EnclosableObject : public WorldObject
{
public:
	EnclosableObject(const std::vector<SpriteAnimation*>& animations);

	virtual void OnEnclosed() = 0;

	virtual ~EnclosableObject() = default;

protected:
	float lastEnclosedTime = 0.0f;
	float enclosedTimer = 0.0f;
	float enclosedTimeLimit = 3.0f;
};