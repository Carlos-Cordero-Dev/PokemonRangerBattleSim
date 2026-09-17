
#pragma once

#include <cmath>

#include "action.h"
#include "action_values.h"
#include "pokemon.h"
#include "world.h"

#include "attack_effect/attack_effect_database.h"
#include "attack_effect/projectile_object_attack_effect.h"
#include "projectile_object.h"
#include "animation_database.h"

class SpawnProjectileObjectAction : public Action {
public:
    std::string projectileObjectType;
    std::unique_ptr<FloatValue> offsetX;
    std::unique_ptr<FloatValue> offsetY;
    std::unique_ptr<FloatValue> dirX;
    std::unique_ptr<FloatValue> dirY;
    std::unique_ptr<FloatValue> speed;
    std::unique_ptr<FloatValue> spawnDelaySec;
    bool aimAtPlayableCenter = false;
    bool useFacingDirection = false;
    std::unique_ptr<FloatValue> spreadDegrees;

    void Execute(WorldObject* owner, float dt) override {

        Pokemon* p = static_cast<Pokemon*>(owner);
        auto& bb = p->stateMachine->blackboard;

        const ProjectileObjectAttackEffect* po_af = (ProjectileObjectAttackEffect*) AttackEffectDatabase::Instance().Get(projectileObjectType);

		AnimationDatabase& animDB = AnimationDatabase::Instance();

        const Vector2 spawnPosition = {
            p->position.x + offsetX->Get() * p->scale,
            p->position.y + offsetY->Get() * p->scale
        };

        Vector2 normDir;

        if (aimAtPlayableCenter) {
            const Rectangle area = GameManager::GetInstance().playableArea;
            const Vector2 center = {
                area.x + area.width * 0.5f,
                area.y + area.height * 0.5f
            };
            normDir = Vector2Normalize({
                center.x - spawnPosition.x,
                center.y - spawnPosition.y
                });
            if (normDir.x == 0.0f && normDir.y == 0.0f) {
                switch (p->facingDir) {
                case FacingDirection::UpLeft:    normDir = { -1.0f, -1.0f }; break;
                case FacingDirection::UpRight:   normDir = { 1.0f, -1.0f }; break;
                case FacingDirection::DownLeft:  normDir = { -1.0f, 1.0f }; break;
                case FacingDirection::DownRight: normDir = { 1.0f, 1.0f }; break;
                default:                         normDir = { 1.0f, 1.0f }; break;
                }
                normDir = Vector2Normalize(normDir);
            }
        }
        else if (useFacingDirection) {
            switch (p->facingDir) {
            case FacingDirection::UpLeft:    normDir = { -1.0f, -1.0f }; break;
            case FacingDirection::UpRight:   normDir = { 1.0f, -1.0f }; break;
            case FacingDirection::DownLeft:  normDir = { -1.0f, 1.0f }; break;
            case FacingDirection::DownRight: normDir = { 1.0f, 1.0f }; break;
            default:                         normDir = { -1.0f, 1.0f }; break;
            }

            const float spreadRadians = spreadDegrees->Get() * DEG2RAD;
            const float angleOffset = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * spreadRadians;
            const float cosine = cosf(angleOffset);
            const float sine = sinf(angleOffset);
            normDir = Vector2Normalize({
                normDir.x * cosine - normDir.y * sine,
                normDir.x * sine + normDir.y * cosine
                });
        }
        else {
            normDir = Vector2Normalize({ dirX->Get(), dirY->Get() });
        }

        if (aimAtPlayableCenter || useFacingDirection) {
            const float spreadRadians = spreadDegrees->Get() * DEG2RAD;
            const float angleOffset = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * spreadRadians;
            const float cosine = cosf(angleOffset);
            const float sine = sinf(angleOffset);
            normDir = Vector2Normalize({
                normDir.x * cosine - normDir.y * sine,
                normDir.x * sine + normDir.y * cosine
                });
        }

        if (normDir.x != 0.0f || normDir.y != 0.0f) {
            if (normDir.x < 0.0f)
                p->SetFacingDirection(normDir.y < 0.0f
                    ? FacingDirection::UpLeft
                    : FacingDirection::DownLeft);
            else
                p->SetFacingDirection(normDir.y < 0.0f
                    ? FacingDirection::UpRight
                    : FacingDirection::DownRight);
        }


		SpriteAnimation* anim = new SpriteAnimation(animDB.GetAnimationDataFromName(po_af->visualAnimation));
        ProjectileObject* po = new ProjectileObject(
            anim,
            0.0f,
            po_af->lifetimeSec
        );
        po->position.x = p->position.x + offsetX->Get() * po_af->scale;
		po->position.y = p->position.y + offsetY->Get() * po_af->scale;
		po->scale = po_af->scale;
        po->rotationDeg = std::atan2(normDir.y, normDir.x) * RAD2DEG
            + po_af->rotationOffsetDeg;
		po->velocity = { normDir.x * speed->Get(), normDir.y * speed->Get() };

        GameManager& gameManager = GameManager::GetInstance();
		//gameManager.activeHazardObjects.push_back(ho);
		gameManager.allWorldObjs.push_back(po);

        printf("Projectile object spawned at (%f, %f)\n", owner->position.x, owner->position.y);

    }
};