
#pragma once

#include "sprites.h"
#include "pokemon.h"

class DrawWarningAction : public Action {
public:
    std::unique_ptr<FloatValue> offsetX, offsetY;

    void Execute(WorldObject* owner, float dt) override {

        Pokemon* p = static_cast<Pokemon*>(owner);

        p->drawWariningSign = true;
        p->warningSignOffset.x = offsetX->Get();
        p->warningSignOffset.y = offsetY->Get();

    }
};