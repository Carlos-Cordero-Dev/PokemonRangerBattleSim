
#pragma once

#include <raylib.h>

#include "action.h"
#include "action_values.h"
#include "world_object.h"

class MoveTowardsTargetAction : public Action {
public:
    std::unique_ptr<FloatValue> speed;


    void Execute(WorldObject* owner, float dt) override {

        Pokemon* p = static_cast<Pokemon*>(owner);
        auto& bb = p->stateMachine->blackboard;

        Vector2 target = bb.vectors["target"];

        Vector2 dir = {
           target.x - owner->position.x,
           target.y - owner->position.y
        };

        float len = sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 0.001f) {
            dir.x /= len;
            dir.y /= len;

            if (dir.x < 0.0f) p->SetFacingDirection(dir.y < 0.0f ? FacingDirection::UpLeft : FacingDirection::DownLeft);
            else p->SetFacingDirection(dir.y < 0.0f ? FacingDirection::UpRight : FacingDirection::DownRight);
        }

        

        owner->position.x += dir.x * speed->Get() * dt;
        owner->position.y += dir.y * speed->Get() * dt;
    }
};