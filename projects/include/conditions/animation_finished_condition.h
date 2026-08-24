
#pragma once

#include "conditions/condition.h"
#include "world_object.h"
#include "pokemon.h"

class AnimationFinishedCondition : public Condition {
public:

    bool Evaluate(WorldObject* owner) override {
        Pokemon* p = static_cast<Pokemon*>(owner);
        auto& bb = p->stateMachine->blackboard;

        return p->animations[p->animationState]->finished;
    }
};