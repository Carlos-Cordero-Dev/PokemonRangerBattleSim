
#include "pokemon.h"

#include "timer.h"

Pokemon::Pokemon(const std::vector<SpriteAnimation*>& animations) : EnclosableObject(animations)
{

}

void Pokemon::AssignStateMachine(StateMachine* stateMachine)
{
	_stateMachine = stateMachine;
	_stateMachine->owner = this;
	for (State* ownedState : _stateMachine->ownedStates)
	{
		ownedState->owner = this;
	}
}

void Pokemon::Update()
{
	if (_stateMachine)
	{
		_stateMachine->Update(Timer::GetInstance().GetDeltaTime());
	}

	WorldObject::Update();
	
}

void Pokemon::OnEnclosed()
{
	printf("enclosed pokimon\n");
}