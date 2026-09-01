
#pragma once

#include <raylib.h>

#include "action.h"
#include "action_values.h"
#include "world_object.h"
#include "pokemon.h"

class StartTimerAction : public Action {
public:
    std::string name;
    std::unique_ptr<FloatValue> duration;

    void Execute(WorldObject* owner, float dt) override {
        Pokemon* p = static_cast<Pokemon*>(owner);
        auto& bb = p->stateMachine->blackboard;

        bb.floats[name] = 0.0f;
        bb.floats[name + "_duration"] = duration->Get();
    }
};
