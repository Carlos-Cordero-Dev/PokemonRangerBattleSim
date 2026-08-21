
#pragma once

#include "projectile_object.h"
#include "timer.h"

void ProjectileObject::Update() 
{
	if (isActive)
	{
		WorldObject::Update();

		float dt = Timer::GetInstance().GetDeltaTime();

		lifetimeSec -= dt;
		if (lifetimeSec <= 0.0f) {
			isActive = false;
		}

		position.x += velocity.x * dt;
		position.y += velocity.y * dt;
	}
}


void ProjectileObject::Draw()
{
	WorldObject::Draw();
}