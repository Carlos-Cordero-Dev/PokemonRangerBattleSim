
#include "states/idle_state.h"
#include "states/move_state.h"

#include "world_object.h"


void IdleState::OnEnter()  
{
	waitTimer = 1.0f;
	printf("enter idle state\n");
}

State* IdleState::OnUpdate(float dt)
{
	waitTimer -= dt;

	if (waitTimer <= 0.0f)
	{
		return on_idle_timer_finished;
	}
	return nullptr;
}

void IdleState::OnExit() 
{
	waitTimer = 1.0f;
	printf("exit idle state\n");
}
