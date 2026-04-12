
#pragma once

#include "raylib.h"

#include "action.h"
#include "action_values.h"
#include "world_object.h"

class SetTargetAction : public Action {
public:
    std::unique_ptr<FloatValue> targetX, targetY;


    void Execute(WorldObject* owner, float dt) override {

        Pokemon* p = static_cast<Pokemon*>(owner);
        p->stateMachine->blackboard.vectors["target"] = {
            targetX->Get(),
            targetY->Get()
        };
    }
};