
#pragma once

#include "state_machine.h"

class AttackState : public State
{
public:
	float waitTimer = 0.0f;

	State* on_attack_completed = nullptr;

public:
	void OnEnter() override;
	State* OnUpdate(float dt) override;
	void OnExit() override;
};

