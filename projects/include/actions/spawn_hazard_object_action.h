
#pragma once

#include "action.h"
#include "action_values.h"
#include "pokemon.h"
#include "hitbox.h"
#include "world.h"

#include "attack_effect/attack_effect_database.h"
#include "attack_effect/hazard_object_attack_effect.h"
#include "hazard_object.h"
#include "animation_database.h"

class SpawnHazardObjectAction : public Action {
public:
    std::string hazardObjectType;
    std::unique_ptr<FloatValue> offsetX;
    std::unique_ptr<FloatValue> offsetY;
    std::unique_ptr<FloatValue> spawnDelaySec;

    void Execute(WorldObject* owner, float dt) override {

        Pokemon* p = static_cast<Pokemon*>(owner);
        auto& bb = p->stateMachine->blackboard;

        const HazardObjectAttackEffect* ho_af = (HazardObjectAttackEffect*) AttackEffectDatabase::Instance().Get(hazardObjectType);

		AnimationDatabase& animDB = AnimationDatabase::Instance();

        //TODO: it probably doesnt make sense that the hitbox is independent of the object
        Hitbox* h = new Hitbox(
            p->position.x + offsetX->Get() * p->scale,
            p->position.y + offsetY->Get() * p->scale,
            ho_af->width * p->scale,
            ho_af->height * p->scale,
            { 0.0f,0.0f },
            spawnDelaySec->Get()
        );
		SpriteAnimation* anim = new SpriteAnimation(animDB.GetAnimationDataFromName(ho_af->visualAnimation));
		HazardObject* ho = new HazardObject(anim,0.0f, ho_af->lifetimeSec,h);
		
        ho->position.x = p->position.x + offsetX->Get() * p->scale;
		ho->position.y = p->position.y + offsetY->Get() * p->scale;
		ho->scale = p->scale;

        GameManager& gameManager = GameManager::GetInstance();
        gameManager.activeHitboxes.push_back(h);
		gameManager.activeHazardObjects.push_back(ho);
		gameManager.allWorldObjs.push_back(ho);

        printf("Hazard object spawned at (%f, %f)\n", owner->position.x, owner->position.y);

    }
};