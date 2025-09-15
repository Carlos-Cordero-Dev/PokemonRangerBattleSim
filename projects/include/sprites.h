
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
	SpriteAnimation(AnimationData* animData) { this->animData = animData; };

	void Update();
	void Draw(int posX, int posY);
	void DrawRotScale(int posX, int posY, float rotDeg, float scale);

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
	AnimationData* animData = nullptr;

private:
	SpriteAnimation();
private:

	int currentFrame = 0;
	int framesPassedSinceLastAnimUpdate = 0;
	int lastRealFrame = 0;

	float timePassed = 0.0f;
	float totalTimePassed = 0.0f;
};



 