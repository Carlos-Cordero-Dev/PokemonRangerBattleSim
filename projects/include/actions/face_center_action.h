
#pragma once

#include "raylib.h"

#include "world.h"
#include "world_object.h"

#include "pokemon.h"

class FaceCenterAction : public Action {
public:

    void Execute(WorldObject* owner, float dt) override {

        Pokemon* p = static_cast<Pokemon*>(owner);

        const Rectangle area = GameManager::GetInstance().playableArea;
        const float halfWidth = owner->boundingBox.width * 0.5f;
        const float halfHeight = owner->boundingBox.height * 0.5f;
        const float centerX = area.x + area.width * 0.5f;
        const float centerY = area.y + area.height * 0.5f;

        const Vector2 center = {
            centerX,
            centerY
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

        if (direction.x < 0.0f) p->SetFacingDirection(direction.y < 0.0f ? FacingDirection::UpLeft : FacingDirection::DownLeft);
        else p->SetFacingDirection(direction.y < 0.0f ? FacingDirection::UpRight : FacingDirection::DownRight);
    }
};