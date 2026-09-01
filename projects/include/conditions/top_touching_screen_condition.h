
#pragma once

#include "conditions/condition.h"
#include "world.h"
#include "world_object.h"
#include "pokemon.h"

class TopTouchingScreenCondition : public Condition {
public:
    std::string name;

    bool Evaluate(WorldObject* owner) override {
        Pokemon* p = static_cast<Pokemon*>(owner);
        auto& bb = p->stateMachine->blackboard;

		WorldObject* topObject = GameManager::GetInstance().topObjectPtr;

        return (topObject->position.x != -100 && topObject->position.y != -100);
    }
};