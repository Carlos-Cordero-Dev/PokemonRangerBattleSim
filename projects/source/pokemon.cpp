
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

	UpdateBBoxPosition();

	//printf("curr anim state %d\n", (int)animationState);
	animations[animationState]->Update(position.x,position.y);
}

void Pokemon::Draw()
{
	//TODO: technically this boundbox w/h should be the w/h of the sprite, not the bb
	animations[animationState]->DrawRotScale(
		position.x - boundingBox.width * scale / 2,
		position.y - boundingBox.height * scale / 2,
		rotationDeg, scale);

	//debug draw bounding box
	//DrawRectangleLines(
	//	boundingBox.x - boundingBox.width * scale / 2 ,
	//	boundingBox.y - boundingBox.height * scale / 2,
	//	boundingBox.width * scale,boundingBox.height * scale,
	//	RED
	//);
	DrawRectangleLinesEx(boundingBox, 1.0f, RED);

	//debug center position
	DrawCircleV(position, 5.5f, GREEN);
}

void Pokemon::OnEnclosed()
{
	printf("enclosed pokimon\n");
}

void Pokemon::Cleanup()
{
	
}