
#include "sprites.h"
#include "timer.h"
#include "world.h"
#include "hitbox.h"
#include "world.h"

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


void SpriteAnimation::Update(int posX, int posY)
{
	//inspired by: https://github.com/jkatsanis/SpriteEngineUI/blob/main/Engine/Engine/Core/Source/Sprite/Components/Animator/Animation.cpp
	
	float deltaTime = Timer::GetInstance().GetDeltaTime();
	this->timePassed += deltaTime;
	this->totalTimePassed += deltaTime;

	float keyframeDelay = animData_->keyframes[currentFrame].delaySec;
	
	
	// === APPLY DELAY ===
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
			if (currentFrame >= animData_->totalFrames)
			{
				currentFrame = 0;
				finished = true;
			}
			//potentially stop if not needed to loop?
		}
	}
	else 
	{
		//printf("\nDefault -1 keyframe");
	}

	// === SPAWN KEYFRAME HITBOXES ===
	for (const AnimationEvent& event : animData_->keyframes[currentFrame].events) {
		if (event.type == AnimationEventType::MeleeHitbox) {

			HitboxAnimationEvent& hbEvent = (HitboxAnimationEvent&)event;
			//TODO: SPAWN HITBOX
			//printf("\nSpawned hitbox from animation event at frame %d", currentFrame);

			GameManager& gm = GameManager::GetInstance();
			gm.singleFrameHitboxes.push_back(new Hitbox(
				posX + hbEvent.offsetX,
				posY + hbEvent.offsetY,
				hbEvent.width,
				hbEvent.height,
				{ 0.0f, 0.0f },
				0.0f
			));
		}
	}

}
void SpriteAnimation::Draw(int posX, int posY)
{
	if (animData_)
	{
		int textureIndex = animData_->keyframes[currentFrame].textureIndex;
		DrawTexture(animData_->textures[textureIndex].texture, posX, posY, WHITE);
	}
}

void SpriteAnimation::DrawRotScale(int posX, int posY, float rotDeg, float scale)
{
	if (animData_)
	{
		Vector2 pos = { posX, posY };
		const int textureIndex = animData_->keyframes[currentFrame].textureIndex;
		DrawTextureEx(animData_->textures[textureIndex].texture, pos, rotDeg, scale, WHITE);
	}
}

void SpriteAnimation::Reset()
{
	currentFrame = 0;
	timePassed = 0.0f;
	totalTimePassed = 0.0f;
	finished = false;
}