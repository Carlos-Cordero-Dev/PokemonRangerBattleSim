
#include "pokemon.h"
#include "timer.h"
#include "world.h"
#include "state_machine.h"

Pokemon::Pokemon(const std::vector<SpriteAnimation*>& animations) : EnclosableObject(animations)
{

}

void Pokemon::AssignStateMachine(StateMachine* stateMachine)
{
	this->stateMachine = stateMachine;
	stateMachine->owner = this;
	for (State* ownedState : stateMachine->ownedStates)
	{
		ownedState->owner = this;
	}
}

void Pokemon::SetAnimationState(const std::string& animationName)
{
	if (animationName == "idle")
		animationState = AnimationState::kIdle;
	else if (animationName == "move")
		animationState = AnimationState::kMove;
	else if (animationName == "attack")
		animationState = AnimationState::kAttack;
}
void Pokemon::Update()
{
	if (stateMachine)
	{
		stateMachine->Update(Timer::GetInstance().GetDeltaTime());
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
void Pokemon::OnEnclosedSetCenter(Vector2* newCenter)
{
	OnEnclosed();

	newCenter->x = position.x;
	newCenter->y = position.y;
}


void Pokemon::Cleanup()
{
	
}