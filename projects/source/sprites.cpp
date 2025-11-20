
#include "sprites.h"
#include "timer.h"

#include "animation_database.h"

SpriteAnimation::SpriteAnimation(const SpriteAnimation& other)
{
	this->loop = other.loop;
	this->animData_ = other.animData_;

	this->currentFrame = other.currentFrame;
	//reset timers
	this->framesPassedSinceLastAnimUpdate = 0;
	this->lastRealFrame = 0;

	this->timePassed = 0.0f;
	this->totalTimePassed = 0.0f;
}


void SpriteAnimation::Update()
{
	//inspired by: https://github.com/jkatsanis/SpriteEngineUI/blob/main/Engine/Engine/Core/Source/Sprite/Components/Animator/Animation.cpp
	
	float deltaTime = Timer::GetInstance().GetDeltaTime();
	this->timePassed += deltaTime;
	this->totalTimePassed += deltaTime;

	float keyframeDelay = animData_->keyframes[currentFrame].delaySec;
	//check if not at default -1.0f delay keyframe
	if (keyframeDelay > 0.0f)
	{
		float condition = keyframeDelay * 1.0f/100.0f/*ms conversion*/;
		if (this->timePassed >= condition)
		{
			//reset internal keyframe timer
			this->timePassed = 0.0f;

			//move to next keyframe
			currentFrame++;
			if (currentFrame > animData_->numberOfTextures - 1) currentFrame = 0;
			//potentially stop if not needed to loop?
		}
	}
	else 
	{
		//printf("\nDefault -1 keyframe");
	}

}
void SpriteAnimation::Draw(int posX, int posY)
{
	DrawTexture(animData_->textures[currentFrame].texture, posX, posY, WHITE);
}

void SpriteAnimation::DrawRotScale(int posX, int posY, float rotDeg, float scale)
{
	Vector2 pos = { posX, posY };
	DrawTextureEx(animData_->textures[currentFrame].texture, pos, rotDeg, scale , WHITE);
}
