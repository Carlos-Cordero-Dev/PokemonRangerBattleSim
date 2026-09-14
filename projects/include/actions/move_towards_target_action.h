
#pragma once

#include <raylib.h>

#include <algorithm>
#include <cmath>

#include "action.h"
#include "action_values.h"
#include "world.h"
#include "world_object.h"

class MoveTowardsTargetAction : public Action {
public:
    std::unique_ptr<FloatValue> speed;


    void Execute(WorldObject* owner, float dt) override {

        Pokemon* p = static_cast<Pokemon*>(owner);
        auto& bb = p->stateMachine->blackboard;

        Vector2 target = bb.vectors["target"];
        const Rectangle playableArea = GameManager::GetInstance().playableArea;
        if (playableArea.width > 0.0f && playableArea.height > 0.0f) {
            const float halfWidth = owner->boundingBox.width * 0.5f;
            const float halfHeight = owner->boundingBox.height * 0.5f;
            const float minX = playableArea.x + halfWidth;
            const float maxX = playableArea.x + playableArea.width - halfWidth;
            const float minY = playableArea.y + halfHeight;
            const float maxY = playableArea.y + playableArea.height - halfHeight;

            target.x = minX <= maxX
                ? std::clamp(target.x, minX, maxX)
                : playableArea.x + playableArea.width * 0.5f;
            target.y = minY <= maxY
                ? std::clamp(target.y, minY, maxY)
                : playableArea.y + playableArea.height * 0.5f;
            bb.vectors["target"] = target;
        }

        Vector2 dir = {
           target.x - owner->position.x,
           target.y - owner->position.y
        };
        const float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        const float movementDistance = std::max<float>(0.0f, speed->Get() * dt);
        if (len <= movementDistance) {
            owner->position = target;
            return;
        }
        float moveDistance = speed->Get() * dt;
        if (len <= moveDistance)
        {
            owner->position = target;
            return;
        }

        dir.x /= len;
        dir.y /= len;

        if (dir.x < 0.0f) p->SetFacingDirection(dir.y < 0.0f ? FacingDirection::UpLeft : FacingDirection::DownLeft);
        else p->SetFacingDirection(dir.y < 0.0f ? FacingDirection::UpRight : FacingDirection::DownRight);
        
        owner->position.x += dir.x * movementDistance;
        owner->position.y += dir.y * movementDistance;
    }
};