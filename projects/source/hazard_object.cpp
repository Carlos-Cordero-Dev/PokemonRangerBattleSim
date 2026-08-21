
#pragma once

#include "hazard_object.h"
#include "timer.h"

void HazardObject::Update() 
{
	if (isActive)
	{
		WorldObject::Update();

		float dt = Timer::GetInstance().GetDeltaTime();

		lifetimeSec -= dt;
		if (lifetimeSec <= 0.0f) {
			isActive = false;
		}
	}
}


void HazardObject::Draw()
{
	WorldObject::Draw();
}