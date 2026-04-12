
#pragma once

//#include "world_object.h"
class WorldObject; //fd

class Action {
public:
    virtual void Execute(WorldObject* owner,float dt) = 0;
    virtual ~Action() = default;
};