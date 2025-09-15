
#include "animation_database.h"

#include <string>
#include <unordered_map>
#include <filesystem>

#include "raylib.h"

#include "constants.h"
#include "sprites.h"

void AnimationDatabase::LoadAnimDataFromFolder(const std::string& basePathFromResourceFolder,
	int numOfAnimationsInFolder, int numOfTexturesPerAnimation, const std::string& texNamePrefix) {

	std::vector<AnimationData*> animations;
	animations.resize(numOfAnimationsInFolder);

	for (int i = 0; i < numOfAnimationsInFolder; i++)
	{
		animations[i] = new AnimationData();

		//default one -1 keyframe so it never advances
		animations[i]->keyframes.resize(1);
		animations[i]->keyframes[0].delaySec = -1.0f;
	}

	int currTextureIndex = 0;
	int currSpriteAnimIndex = 0;

	std::string basePath = RESOURCES_FOLDER + basePathFromResourceFolder;

	// Traverse directories and subdirectories
	for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(basePath)) {
		if (dirEntry.is_regular_file()) {
			// Get file path and check if it's a texture (by extension, e.g., .png, .jpg)
			std::string filePath = dirEntry.path().string();
			std::string extension = dirEntry.path().extension().string();


			// Skip files that are not textures or keyframe data
			if (extension != ".png" && extension != ".jpg" && extension != ".jpeg" /*&& extension != ".prkf"*/) {
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

			std::replace(filePath.begin(), filePath.end(), '\\', '/');
			Texture2D texture = LoadTexture(filePath.c_str());

			// Add the texture to the vector with its name
			if (!texNamePrefix.empty())
			{
				textureName = textureName.append(texNamePrefix);
			}

			animations[currSpriteAnimIndex]->textures.push_back({ texture, textureName });

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
						animations[i]->numberOfTextures = animations[i]->textures.size();
					}

					//this should only happen in the literal end step of the algorithm and if this is ever reached while the 
					//folder is not completely traversed something is very wrong

						//move from vector to unordered map
					for (AnimationData* animation : animations)
					{
						animData.emplace(std::pair<std::string, AnimationData*>(animation->textures[0].name, animation));
					}
					return;
				}
			}
		}
	}

	//algo never reaches this point , always from return above ^
}

AnimationData* AnimationDatabase::GetAnimationDataFromName(const std::string& name)
{
	auto it = animData.find(name);
	if (it != animData.end()) {
		return it->second;
	}
	return nullptr; // not found
}
