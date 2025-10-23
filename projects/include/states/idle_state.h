
#pragma once

#include "state_machine.h"

class IdleState : public State
{
public:
	float waitTimer = 0.0f;

	State* on_idle_timer_finished = nullptr;

public:
	void OnEnter() override;
	State* OnUpdate(float dt) override;
	void OnExit() override;
};

