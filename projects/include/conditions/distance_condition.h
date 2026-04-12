
#pragma once

#include "conditions/condition.h"
#include "world_object.h"
#include "pokemon.h"

class DistanceToTargetLess : public Condition {
public:
    float value;

    bool Evaluate(WorldObject* owner) override {
        Pokemon* p = static_cast<Pokemon*>(owner);
        auto& bb = p->stateMachine->blackboard;

        Vector2 target = bb.vectors["target"];

        float dx = target.x - owner->position.x;
        float dy = target.y - owner->position.y;

        float dist = sqrt(dx * dx + dy * dy);
        return dist < value;
    }
};