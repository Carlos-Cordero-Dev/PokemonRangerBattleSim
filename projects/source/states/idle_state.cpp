
#include "states/idle_state.h"
#include "states/move_state.h"

#include "world_object.h"


void IdleState::OnEnter()  
{
	waitTimer = 3.0f;
}

State* IdleState::OnUpdate(float dt)
{
	waitTimer -= dt;
	printf("updating idle\n");

	if (waitTimer <= 0.0f)
	{
		return on_idle_timer_finished;
	}
	return nullptr;
}

void IdleState::OnExit() 
{

}
