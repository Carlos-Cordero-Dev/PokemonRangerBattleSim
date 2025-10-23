
#include "states/attack_state.h"

#include "world_object.h"


void AttackState::OnEnter()
{
	waitTimer = 3.0f;
}

State* AttackState::OnUpdate(float dt)
{
	waitTimer -= dt;
	if (waitTimer <= 0.0f)
	{
		return on_attack_completed;
	}

	return nullptr;
}

void AttackState::OnExit()
{

}