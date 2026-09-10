
#include "pokemon.h"
#include "timer.h"
#include "world.h"
#include "state_machine.h"
#include "animation_database.h"

Pokemon::Pokemon(const AnimationSet& animations) : EnclosableObject({}) 
{
	facingDir = FacingDirection::DownLeft;
	animationState = "idle";
	animationSet = animations;
	enclosedTimeLimit = 3.0f;


	const AnimationVariant* initialAnimation = ResolveAnimation();
	if (!initialAnimation || !initialAnimation->data) {
		printf("Pokemon has no valid idle animation\n");
		return;
	}

	animationPlayer.SetAnimationData(initialAnimation->data, false);
	const Vector2 textureSize = animationPlayer.GetCurrentTextureSize();

	boundingBox.width = textureSize.x;
	baseWidth = boundingBox.width;
	boundingBox.height = textureSize.y;
	baseHeight = boundingBox.height;
}

void Pokemon::AssignStateMachine(StateMachine* stateMachine)
{
	if (stateMachine)
	{
		this->stateMachine = stateMachine;
		stateMachine->owner = this;
		for (State* ownedState : stateMachine->ownedStates)
			ownedState->owner = this;
	}
	else printf("tried to assign null state machine\n");
}

void Pokemon::SetAnimationState(const std::string& animationName)
{
	if (animationName == animationState)
		return;

	if (animationSet.find(animationName) == animationSet.end()) {
		printf("Unknown Pokemon animation state '%s'\n", animationName.c_str());
		return;
	}

	animationState = animationName;
	restartAnimation = true;
}

void Pokemon::SetFacingDirection(FacingDirection direction)
{
	facingDir = direction;
}

bool Pokemon::IsAnimationFinished() const
{
	return !restartAnimation && animationPlayer.IsFinished();
}

const AnimationVariant* Pokemon::ResolveAnimation() const
{
	auto actionIt = animationSet.find(animationState);
	if (actionIt == animationSet.end() || actionIt->second.empty())
		return nullptr;

	auto directionIt = actionIt->second.find(facingDir);
	if (directionIt != actionIt->second.end())
		return &directionIt->second;

	directionIt = actionIt->second.find(FacingDirection::DownLeft);
	if (directionIt != actionIt->second.end())
		return &directionIt->second;

	return &actionIt->second.begin()->second;
}

void Pokemon::Update()
{
	if (stateMachine)
	{
		stateMachine->Update(Timer::GetInstance().GetDeltaTime());
	}

	UpdateBBox();

	const AnimationVariant* animation = ResolveAnimation();

	animationPlayer.SetAnimationData(animation->data, !restartAnimation);
	restartAnimation = false;
	animationPlayer.Update(position.x, position.y);



}

void Pokemon::Draw()
{
	//TODO: technically this boundbox w/h should be the w/h of the sprite, not the bb

	const AnimationVariant* animation = ResolveAnimation();
	if (animation && animation->data)
	{
		animationPlayer.DrawRotScaleCentered(
			position.x,
			position.y,
			rotationDeg, scale, animation->flipX);
	}

	DrawRectangleLinesEx(boundingBox, 1.0f, RED);
	DrawCircleV(position, 5.5f, GREEN);


	// HEALTHBAR
	if (isEnclosed)
	{
		DrawRectangle(position.x, boundingBox.y, 50, 5, DARKGRAY);
		// Draw the health bar foreground based on healthPercentage
		DrawRectangle(position.x, boundingBox.y, (int)(50 * currHealth / maxHealth), 5, RED);

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