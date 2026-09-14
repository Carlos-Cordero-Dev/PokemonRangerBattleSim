
#pragma once

#include "particle.h"

class ParticleSystem
{
public:

	virtual void step(float dt) = 0;
	virtual void Reset() = 0;
	virtual void Draw() = 0;
};