
#pragma once

#include "raylib.h"

#include "action.h"
#include "action_values.h"
#include "world.h"
#include "pokemon.h"

class SetTargetToTouchAction : public Action {
public:

    void Execute(WorldObject* owner, float dt) override {

		WorldObject* topObject = GameManager::GetInstance().topObjectPtr;
        Pokemon* p = static_cast<Pokemon*>(owner);
        p->stateMachine->blackboard.vectors["target"] = {
            topObject->position.x,
            topObject->position.y
        };
    }
};