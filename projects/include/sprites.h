
#pragma once

#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>

#include "raylib.h"

#include "constants.h"

class AnimationData;

// NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
struct TextureData {
	Texture2D texture;
	std::string name;
};

class SpriteAnimation
{

public:
	//TODO: if nullptr animData prolly should create a no sprite sprite
	explicit SpriteAnimation(AnimationData* animData = nullptr) : animData_(animData) {};
	SpriteAnimation(const SpriteAnimation& other);

	void SetAnimationData(AnimationData* animData, bool preservePlayback);
	void Update(int posX, int posY);
	void Draw(int posX, int posY);
	void DrawRotScale(int posX, int posY, float rotDeg, float scale, bool flipX = false);
	void DrawRotScaleCentered(float centerX, float centerY, float rotDeg, float scale, bool flipX = false);
	Vector2 GetCurrentTextureSize() const;

	bool IsFinished() const { return finished; }
	void Reset();
	//TODO: if pause is ever needed these are useful
	// 	enum class AnimState
	//{
	//	kPlaying = 1,
	//		kFrozen,
	//		kMAXStates
	//};
	//void Pause(); useAnimState pls
	//void Play();

	bool loop = false;
	bool finished = false;
	AnimationData* animData_ = nullptr;
	int currentFrame = 0;

private:

	int framesPassedSinceLastAnimUpdate = 0;
	int lastRealFrame = 0;

	float timePassed = 0.0f;
	float totalTimePassed = 0.0f;
};



 