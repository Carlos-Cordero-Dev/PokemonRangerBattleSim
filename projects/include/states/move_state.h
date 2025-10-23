
#pragma once

#include "state_machine.h"

#include <stdio.h>
#include <raylib.h>

class MoveState : public State
{
public:
	float waitTimer = 0.0f;
	Vector2 direction;

	State* on_move_action_completed = nullptr;

public:
	void OnEnter() override;
	State* OnUpdate(float dt) override;
	void OnExit() override;
};

