
#pragma once

#include "conditions/condition.h"
#include "world_object.h"
#include "pokemon.h"

class TimerFinishedCondition : public Condition {
public:
    std::string name;

    bool Evaluate(WorldObject* owner) override {
        Pokemon* p = static_cast<Pokemon*>(owner);
        auto& bb = p->stateMachine->blackboard;

        return bb.floats[name] >= bb.floats[name + "_duration"];
    }
};