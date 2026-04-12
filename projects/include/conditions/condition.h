
#pragma once

//#include "world_object.h"
class WorldObject; //fd

class Condition {
public:
    virtual bool Evaluate(WorldObject* owner) = 0;
    virtual ~Condition() = default;
};