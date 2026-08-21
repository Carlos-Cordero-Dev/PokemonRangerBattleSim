#pragma once

#include "attack_effect.h"

class ProjectileObjectAttackEffect : public AttackEffect {
public:
    std::string visualAnimation;
    float width = 0.0f;
    float height = 0.0f; 
    float lifetimeSec = 0.0f;
};