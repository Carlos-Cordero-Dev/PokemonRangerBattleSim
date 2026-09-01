
#include "pokemon.h"
#include "timer.h"
#include "world.h"
#include "state_machine.h"

Pokemon::Pokemon(const std::vector<SpriteAnimation*>& animations) : EnclosableObject(animations)
{
	animationState = AnimationState::kIdle;
	enclosedTimeLimit = 3.0f;
}

void Pokemon::AssignStateMachine(StateMachine* stateMachine)
{
	if (stateMachine)
	{
		this->stateMachine = stateMachine;
		stateMachine->owner = this;
		for (State* ownedState : stateMachine->ownedStates)
		{
			ownedState->owner = this;
		}
	}
	else printf("tried to assign null state machine\n");
}

void Pokemon::SetAnimationState(const std::string& animationName)
{
	AnimationState nextState = AnimationState::kIdle;

	if (animationName == "idle")
		nextState = AnimationState::kIdle;
	else if (animationName == "move")
		nextState = AnimationState::kMove;
	else if (animationName == "attack")
		nextState = AnimationState::kAttack;
	if (animationState != nextState)
	{
		animationState = nextState;
		animations[animationState]->Reset();
	}
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
	float centerLeftX = position.x - boundingBox.width * scale / 2;
	float centerTopY = position.y - boundingBox.height * scale / 2;

	animations[animationState]->DrawRotScale(
		centerLeftX,
		centerTopY,
		rotationDeg, scale);

	// HEALTHBAR
	if (isEnclosed)
	{
		DrawRectangle(position.x, centerTopY, 50, 5, DARKGRAY);
		// Draw the health bar foreground based on healthPercentage
		DrawRectangle(position.x, centerTopY, (int)(50 * currHealth / maxHealth), 5, RED);

		enclosedTimer += Timer::GetInstance().GetDeltaTime();
		if(enclosedTimer > enclosedTimeLimit)
		{
			enclosedTimer = 0.0f;
			isEnclosed = false;
		}
	}

	//debug draw bounding box
	//DrawRectangleLines(
	//	boundingBox.x - boundingBox.width * scale / 2 ,
	//	boundingBox.y - boundingBox.height * scale / 2,
	//	boundingBox.width * scale,boundingBox.height * scale,
	//	RED
	//);
	DrawRectangleLinesEx(boundingBox, 1.0f, BLUE);

	//debug center position
	//DrawCircleV(position, 5.5f, GREEN);
}

void Pokemon::OnEnclosed()
{
	printf("enclosed pokimon\n");

	enclosedTimer = 0.0f;
	isEnclosed = true;
}

void Pokemon::OnEnclosedEx(Vector2* newCenter, float incomingDamage)
{
	OnEnclosed();

	newCenter->x = position.x;
	newCenter->y = position.y;

	currHealth -= incomingDamage;

}


void Pokemon::Cleanup()
{
	
}