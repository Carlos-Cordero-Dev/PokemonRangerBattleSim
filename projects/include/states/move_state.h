
#pragma once

#include "state_machine.h"

#include "stdio.h"

class MoveState : public State
{
public:
	State* on_move_action_completed = nullptr;

public:
	void OnEnter() override { printf("entered move\n"); };
	State* OnUpdate(float dt) override {	return nullptr; };
	void OnExit() override {};
};

