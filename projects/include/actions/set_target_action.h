
#pragma once

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>

#include "raylib.h"

#include "action.h"
#include "action_values.h"
#include "world.h"
#include "world_object.h"

class SetTargetAction : public Action {
public:
    std::unique_ptr<FloatValue> targetX, targetY;
    std::unique_ptr<FloatValue> radius;
    std::unique_ptr<FloatValue> travelDistance;
    std::string area;


    void Execute(WorldObject* owner, float dt) override {

        Pokemon* p = static_cast<Pokemon*>(owner);

        if (area == "playable" || area == "through_playable_center") {
            const Rectangle area = GameManager::GetInstance().playableArea;
            const float halfWidth = owner->boundingBox.width * 0.5f;
            const float halfHeight = owner->boundingBox.height * 0.5f;
            const float minX = area.x + halfWidth;
            const float maxX = area.x + area.width - halfWidth;
            const float minY = area.y + halfHeight;
            const float maxY = area.y + area.height - halfHeight;

            if (this->area == "through_playable_center") {
                const Vector2 center = {
                    area.x + area.width * 0.5f,
                    area.y + area.height * 0.5f
                };
                Vector2 direction = {
                    center.x - owner->position.x,
                    center.y - owner->position.y
                };
                const float length = std::sqrt(
                    direction.x * direction.x + direction.y * direction.y);

                if (length <= 1e-6f) {
                    switch (p->facingDir) {
                    case FacingDirection::UpLeft:    direction = { -1.0f, -1.0f }; break;
                    case FacingDirection::UpRight:   direction = { 1.0f, -1.0f }; break;
                    case FacingDirection::DownLeft:  direction = { -1.0f, 1.0f }; break;
                    case FacingDirection::DownRight: direction = { 1.0f, 1.0f }; break;
                    default:                         direction = { 1.0f, 1.0f }; break;
                    }
                    const float facingLength = std::sqrt(
                        direction.x * direction.x + direction.y * direction.y);
                    direction.x /= facingLength;
                    direction.y /= facingLength;
                }
                else {
                    direction.x /= length;
                    direction.y /= length;
                }

                const float distanceToX = direction.x > 1e-6f
                    ? (maxX - owner->position.x) / direction.x
                    : direction.x < -1e-6f
                    ? (minX - owner->position.x) / direction.x
                    : INFINITY;
                const float distanceToY = direction.y > 1e-6f
                    ? (maxY - owner->position.y) / direction.y
                    : direction.y < -1e-6f
                    ? (minY - owner->position.y) / direction.y
                    : INFINITY;
                const float boundaryDistance = std::min<float>(distanceToX, distanceToY);
                const float distance = travelDistance
                    ? std::min<float>(std::max<float>(0.0f, travelDistance->Get()), boundaryDistance)
                    : boundaryDistance;

                p->stateMachine->blackboard.vectors["target"] = {
                    owner->position.x + direction.x * distance,
                    owner->position.y + direction.y * distance
                };
                return;
            }

            const float randomX = static_cast<float>(rand()) / RAND_MAX;
            const float randomY = static_cast<float>(rand()) / RAND_MAX;

            if (radius) {
                const float targetRadius = std::max<float>(0.0f, radius->Get());
                const float angle = randomX * 2.0f * PI;
                const float distance = std::sqrt(randomY) * targetRadius;

                p->stateMachine->blackboard.vectors["target"] = {
                    std::clamp(owner->position.x + std::cos(angle) * distance, minX, maxX),
                    std::clamp(owner->position.y + std::sin(angle) * distance, minY, maxY)
                };
                return;
            }

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