
#pragma once

#include <string>

#include "action.h"
#include "world_object.h"
#include "pokemon.h"

class SetAnimationAction : public Action {
public:
    std::string animationName;

    void Execute(WorldObject* owner, float dt) override {
        Pokemon* owner_pkm = dynamic_cast<Pokemon*>(owner);
        owner_pkm->SetAnimationState(animationName);
    }   
};