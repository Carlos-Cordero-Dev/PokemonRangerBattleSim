
#include "sprites.h"
#include "timer.h"
#include "world.h"
#include "hitbox.h"
#include "raymath.h"
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
	this->finished = false;
}

void SpriteAnimation::SetAnimationData(AnimationData* animData, bool preservePlayback)
{
	if (!animData || animData->keyframes.empty() || animData->textures.empty())
		return;

	animData_ = animData;
	if (!preservePlayback)
	{
		currentFrame = 0;
		timePassed = 0.0f;
		totalTimePassed = 0.0f;
		finished = false;
		return;
	}

	if (currentFrame >= static_cast<int>(animData_->keyframes.size()))
		currentFrame = static_cast<int>(animData_->keyframes.size()) - 1;
}

void SpriteAnimation::Update(int posX, int posY)
{
	//inspired by: https://github.com/jkatsanis/SpriteEngineUI/blob/main/Engine/Engine/Core/Source/Sprite/Components/Animator/Animation.cpp

	if (!animData_ || animData_->keyframes.empty())
		return;
	if (finished && !animData_->loop)
		return;

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
				if (animData_->loop) {
					currentFrame = 0;
				}
				else currentFrame = animData_->totalFrames - 1;

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
	if (!animData_ || animData_->keyframes.empty() || animData_->textures.empty())
		return;
		
	int textureIndex = animData_->keyframes[currentFrame].textureIndex;
	DrawTexture(animData_->textures[textureIndex].texture, posX, posY, WHITE);
	
}

void SpriteAnimation::DrawRotScale(int posX, int posY, float rotDeg, float scale, bool flipX)
{
	if (!animData_ || animData_->keyframes.empty() || animData_->textures.empty())
		return;

	const int textureIndex = animData_->keyframes[currentFrame].textureIndex;
	const Texture2D texture = animData_->textures[textureIndex].texture;
	const Rectangle source = {
		0.0f,
		0.0f,
		flipX ? -static_cast<float>(texture.width) : static_cast<float>(texture.width),
		static_cast<float>(texture.height)
	};
	const Rectangle destination = {
		static_cast<float>(posX),
		static_cast<float>(posY),
		static_cast<float>(texture.width) * scale,
		static_cast<float>(texture.height) * scale
	};

	DrawTexturePro(texture, source, destination, Vector2Zero(), rotDeg, WHITE);
}

void SpriteAnimation::DrawRotScaleCentered(float centerX, float centerY, float rotDeg, float scale, bool flipX)
{
	if (!animData_ || animData_->keyframes.empty() || animData_->textures.empty())
		return;

	const int textureIndex = animData_->keyframes[currentFrame].textureIndex;
	const Texture2D texture = animData_->textures[textureIndex].texture;
	const Rectangle source = {
		0.0f,
		0.0f,
		flipX ? -static_cast<float>(texture.width) : static_cast<float>(texture.width),
		static_cast<float>(texture.height)
	};
	const Rectangle destination = {
		centerX,
		centerY,
		static_cast<float>(texture.width) * scale,
		static_cast<float>(texture.height) * scale
	};
	const Vector2 origin = {
		destination.width / 2.0f,
		destination.height / 2.0f
	};

	DrawTexturePro(texture, source, destination, origin, rotDeg, WHITE);
}

Vector2 SpriteAnimation::GetCurrentTextureSize() const
{
	if (!animData_ || animData_->keyframes.empty() || animData_->textures.empty())
		return Vector2Zero();

	const int textureIndex = animData_->keyframes[currentFrame].textureIndex;
	if (textureIndex < 0 || textureIndex >= static_cast<int>(animData_->textures.size()))
		return Vector2Zero();

	const Texture2D texture = animData_->textures[textureIndex].texture;
	return {
		static_cast<float>(texture.width),
		static_cast<float>(texture.height)
	};
}

void SpriteAnimation::Reset()
{
	currentFrame = 0;
	timePassed = 0.0f;
	totalTimePassed = 0.0f;
	finished = false;
}