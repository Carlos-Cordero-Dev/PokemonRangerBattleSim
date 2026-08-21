
#pragma once

#include "action.h"
#include "action_values.h"
#include "pokemon.h"
#include "hitbox.h"
#include "world.h"

class SpawnHitboxAction : public Action {
public:
    std::unique_ptr<FloatValue> delaySec;
    std::unique_ptr<FloatValue> width;
    std::unique_ptr<FloatValue> height;
    std::unique_ptr<FloatValue> speed;
    std::unique_ptr<FloatValue> dirX;
    std::unique_ptr<FloatValue> dirY;

    void Execute(WorldObject* owner, float dt) override {

        Pokemon* p = static_cast<Pokemon*>(owner);
        auto& bb = p->stateMachine->blackboard;

        Hitbox* h = new Hitbox(
            p->position.x,
            p->position.y,
            width->Get() * p->scale,
            height->Get() * p->scale,
			{ dirX->Get() * speed->Get(), dirY->Get() * speed->Get()},
            delaySec->Get()
        );

        GameManager::GetInstance().activeHitboxes.push_back(h);

        printf("Spawn hitbox at (%f, %f)\n", owner->position.x, owner->position.y);

    }
};