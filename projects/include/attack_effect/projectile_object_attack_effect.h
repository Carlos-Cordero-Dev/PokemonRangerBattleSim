#pragma once

#include "attack_effect.h"

class ProjectileObjectAttackEffect : public AttackEffect {
public:
    std::string visualAnimation;
    float scale = 1.0f;
    float rotationDeg = 0.0f;
    float lifetimeSec = 0.0f;
    float rotationOffsetDeg = 0.0f;
};