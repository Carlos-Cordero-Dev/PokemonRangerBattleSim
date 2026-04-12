
#pragma once

#include <raylib.h>

#include "action.h"
#include "world_object.h"
#include "pokemon.h"

class TickTimerAction : public Action {
public:
    std::string name;

    void Execute(WorldObject* owner, float dt) override {
        Pokemon* p = static_cast<Pokemon*>(owner);
        auto& bb = p->stateMachine->blackboard;        
        
        bb.floats[name] += dt;
    }
};