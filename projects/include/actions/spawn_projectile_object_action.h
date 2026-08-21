
#pragma once

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

    void Execute(WorldObject* owner, float dt) override {

        Pokemon* p = static_cast<Pokemon*>(owner);
        auto& bb = p->stateMachine->blackboard;

        const ProjectileObjectAttackEffect* po_af = (ProjectileObjectAttackEffect*) AttackEffectDatabase::Instance().Get(projectileObjectType);

		AnimationDatabase& animDB = AnimationDatabase::Instance();

		Vector2 normDir = Vector2Normalize({ dirX->Get(), dirY->Get() });
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