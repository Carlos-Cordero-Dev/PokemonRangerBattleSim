
#pragma once
#include "world_object.h"
#include "hitbox.h"

class HazardObject : public WorldObject {
public:

    HazardObject(SpriteAnimation* animation, float damage, float lifetimeSec, Hitbox* hb)
		: WorldObject(animation), damage(damage), lifetimeSec(lifetimeSec), hb(hb) {
	}
    void Update() override;
	void Draw() override;

    float damage = 0.0f;
    float lifetimeSec = 0.0f;
    bool isActive = true;

    Hitbox* hb;
};