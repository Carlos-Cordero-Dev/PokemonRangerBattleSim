
#pragma once

#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>

#include "raylib.h"

#include "constants.h"

// NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
struct TextureData {
	Texture2D texture;
	std::string name;
};

class SpriteAnimation
{
	/*
	update()
	{
		frame++
			foreach(activeanim in activeanims)
		{
			//important to make them DEPENDENT on the framerate
			if(activeanim.state != frozen)
			activeanim.frameAdvace()
		}
	}*/
	enum class AnimState
	{
		kPlaying = 1,
		kFrozen,
		kMAXStates
	};

public:
	void advanceFrame(int frame);
	void Draw(int posX, int posY);
	void DrawRotScale(int posX, int posY, float rotDeg, float scale);

	std::string name;
	std::vector<TextureData> textures;
	int advanceRate = 1; //every how many frames to advance
	int numberOfTextures;
private:
	int id;

	int currentFrame = 0;
	int framesPassedSinceLastAnimUpdate = 0;
	int lastRealFrame = 0;
};

void loadTexturesFromFolder(const std::string& basePathFromResourceFolder, SpriteAnimation** spriteAnims,
	int numOfAnimationsInFolder, int numOfTexturesPerAnimation, const std::string& texNamePrefix);

 