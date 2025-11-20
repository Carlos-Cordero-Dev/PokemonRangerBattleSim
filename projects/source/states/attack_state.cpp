
#include "states/attack_state.h"

#include "world_object.h"
#include "pokemon.h"

void AttackState::OnEnter()
{
	waitTimer = 1.0f;
	printf("enter attack state\n");

	Pokemon* owner_pkm = dynamic_cast<Pokemon*>(owner);
	owner_pkm->animationState = Pokemon::AnimationState::kAttack;
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
	waitTimer = 1.0f;
	printf("exit attack state\n");
}