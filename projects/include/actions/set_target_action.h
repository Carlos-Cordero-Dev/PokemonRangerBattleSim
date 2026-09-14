
#pragma once

#include <cstdlib>

#include "raylib.h"

#include "action.h"
#include "action_values.h"
#include "world.h"
#include "world_object.h"

class SetTargetAction : public Action {
public:
    std::unique_ptr<FloatValue> targetX, targetY;
    bool usePlayableArea = false;


    void Execute(WorldObject* owner, float dt) override {

        Pokemon* p = static_cast<Pokemon*>(owner);

        if (usePlayableArea) {
            const Rectangle area = GameManager::GetInstance().playableArea;
            const float halfWidth = owner->boundingBox.width * 0.5f;
            const float halfHeight = owner->boundingBox.height * 0.5f;
            const float minX = area.x + halfWidth;
            const float maxX = area.x + area.width - halfWidth;
            const float minY = area.y + halfHeight;
            const float maxY = area.y + area.height - halfHeight;
            const float randomX = static_cast<float>(rand()) / RAND_MAX;
            const float randomY = static_cast<float>(rand()) / RAND_MAX;

            p->stateMachine->blackboard.vectors["target"] = {
                minX <= maxX ? minX + randomX * (maxX - minX) : area.x + area.width * 0.5f,
                minY <= maxY ? minY + randomY * (maxY - minY) : area.y + area.height * 0.5f
            };
            return;
        }


        p->stateMachine->blackboard.vectors["target"] = {
            targetX->Get(),
            targetY->Get()
        };
    }
};