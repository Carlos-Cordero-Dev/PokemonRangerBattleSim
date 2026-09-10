
#pragma once

#include <cmath>

#include "action.h"
#include "action_values.h"
#include "pokemon.h"
#include "hitbox.h"
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
    bool useFacingDirection = false;
    std::unique_ptr<FloatValue> spreadDegrees;

    void Execute(WorldObject* owner, float dt) override {

        Pokemon* p = static_cast<Pokemon*>(owner);
        auto& bb = p->stateMachine->blackboard;

        const ProjectileObjectAttackEffect* po_af = (ProjectileObjectAttackEffect*) AttackEffectDatabase::Instance().Get(projectileObjectType);

		AnimationDatabase& animDB = AnimationDatabase::Instance();

        Vector2 normDir;
        if (useFacingDirection) {
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

        //TODO: it probably doesnt make sense that the hitbox is independent of the object
        Hitbox* h = new Hitbox(
            p->position.x + offsetX->Get() * p->scale,
            p->position.y + offsetY->Get() * p->scale,
            po_af->width * p->scale,
            po_af->height * p->scale,
            { normDir.x * speed->Get(), normDir.y * speed->Get() },
            spawnDelaySec->Get()
        );
		SpriteAnimation* anim = new SpriteAnimation(animDB.GetAnimationDataFromName(po_af->visualAnimation));
		ProjectileObject* po = new ProjectileObject(anim, 0.0f, po_af->lifetimeSec, h);

        po->position.x = p->position.x + offsetX->Get() * p->scale;
		po->position.y = p->position.y + offsetY->Get() * p->scale;
		po->scale = p->scale;
		po->velocity = { normDir.x * speed->Get(), normDir.y * speed->Get() };

        GameManager& gameManager = GameManager::GetInstance();
        gameManager.activeHitboxes.push_back(h);
		//gameManager.activeHazardObjects.push_back(ho);
		gameManager.allWorldObjs.push_back(po);

        printf("Projectile object spawned at (%f, %f)\n", owner->position.x, owner->position.y);

    }
};