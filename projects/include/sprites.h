
#pragma once

#include "raylib.h"
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
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

void SpriteAnimation::advanceFrame(int frame)
{
	framesPassedSinceLastAnimUpdate++;
	if (framesPassedSinceLastAnimUpdate >= advanceRate)
	{
		framesPassedSinceLastAnimUpdate = 0;

		currentFrame++;
		if (currentFrame >= numberOfTextures) currentFrame = 0;

		//unused for now
		lastRealFrame = frame;

	}
}
void SpriteAnimation::Draw(int posX, int posY)
{
	DrawTexture(textures[currentFrame].texture, posX, posY, WHITE);
}
void loadTexturesFromFolder(const std::string& basePath, SpriteAnimation** spriteAnims, 
	int numOfAnimationsInFolder, int numOfTexturesPerAnimation, const std::string& texNamePrefix) {

	int currTextureIndex = 0;
	int currSpriteAnimIndex = 0;

	// Traverse directories and subdirectories
	for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(basePath)) {
		if (dirEntry.is_regular_file()) {
			// Get file path and check if it's a texture (by extension, e.g., .png, .jpg)
			std::string filePath = dirEntry.path().string();
			std::string extension = dirEntry.path().extension().string();

			// Skip files that are not textures (you can add more extensions if needed)
			if (extension != ".png" && extension != ".jpg" && extension != ".jpeg") {
				continue;
			}

			// Extract the directory structure (basename)
			std::string relativePath = dirEntry.path().string();
			size_t baseDirPos = relativePath.find(basePath);
			if (baseDirPos != std::string::npos) {
				relativePath = relativePath.substr(baseDirPos + basePath.length() + 1); // Skip the basePath and leading '/'
			}

			// Extract parts of the path to create the name
			size_t lastSlash = relativePath.find_last_of('/');
			std::string category = relativePath.substr(0, lastSlash);  // e.g., "attack"
			std::replace(category.begin(), category.end(), '\\', '_');

			size_t extPos = category.find_last_of('.');
			if (extPos != std::string::npos) {
				category = category.substr(0, extPos);  // Remove ".png", leaving "18"
			}

			std::string textureName = category;
			
			Texture2D texture = LoadTexture(filePath.c_str());

			// Add the texture to the vector with its name
			if (!texNamePrefix.empty())
			{
				textureName = textureName.append(texNamePrefix);
			}
			spriteAnims[currSpriteAnimIndex]->textures.push_back({ texture, textureName });

			currTextureIndex++;
			if (currTextureIndex >= numOfTexturesPerAnimation)
			{
				currTextureIndex = 0;
				currSpriteAnimIndex++;
				if (currSpriteAnimIndex >= numOfAnimationsInFolder)
				{
					//cache number of textures
					for (int i = 0; i < numOfAnimationsInFolder; i++)
					{
						spriteAnims[i]->numberOfTextures = spriteAnims[i]->textures.size();
					}

					//this should only happen in the literal end step of the algorithm and if this is ever reached while the 
					//folder is not completely traversed something is very wrong
					return;
				}
			}
		}
	}

}

void loadTexturesFromFolder(const std::string& basePath, SpriteAnimation** spriteAnims,
	int numOfAnimationsInFolder, int* numOfTexturesPerAnimation, const std::string& texNamePrefix) {

	int currTextureIndex = 0;
	int currSpriteAnimIndex = 0;

	// Traverse directories and subdirectories
	for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(basePath)) {
		if (dirEntry.is_regular_file()) {
			// Get file path and check if it's a texture (by extension, e.g., .png, .jpg)
			std::string filePath = dirEntry.path().string();
			std::string extension = dirEntry.path().extension().string();

			// Skip files that are not textures (you can add more extensions if needed)
			if (extension != ".png" && extension != ".jpg" && extension != ".jpeg") {
				continue;
			}

			// Extract the directory structure (basename)
			std::string relativePath = dirEntry.path().string();
			size_t baseDirPos = relativePath.find(basePath);
			if (baseDirPos != std::string::npos) {
				relativePath = relativePath.substr(baseDirPos + basePath.length() + 1); // Skip the basePath and leading '/'
			}

			// Extract parts of the path to create the name
			size_t lastSlash = relativePath.find_last_of('/');
			std::string category = relativePath.substr(0, lastSlash);  // e.g., "attack"
			std::replace(category.begin(), category.end(), '\\', '_');

			size_t extPos = category.find_last_of('.');
			if (extPos != std::string::npos) {
				category = category.substr(0, extPos);  // Remove ".png", leaving "18"
			}

			std::string textureName = category;

			Texture2D texture = LoadTexture(filePath.c_str());

			// Add the texture to the vector with its name
			if (!texNamePrefix.empty())
			{
				textureName = textureName.append(texNamePrefix);
			}
			spriteAnims[currSpriteAnimIndex]->textures.push_back({ texture, textureName });

			currTextureIndex++;
			if (currTextureIndex >= numOfTexturesPerAnimation[currSpriteAnimIndex])
			{
				currTextureIndex = 0;
				currSpriteAnimIndex++;
				if (currSpriteAnimIndex >= numOfAnimationsInFolder)
				{
					//this should only happen in the literal end step of the algorithm and if this is ever reached while the 
					//folder is not completely traversed something is very wrong
					return;
				}
			}
		}
	}

}
 