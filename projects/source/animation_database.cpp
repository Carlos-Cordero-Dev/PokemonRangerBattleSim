
#include "animation_database.h"

#include <string>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "raylib.h"

#include "constants.h"
#include "sprites.h"

std::vector<std::vector<KeyFrame>> LoadKeyframesFromFile(const std::string& file_path)
{ 

	std::vector<std::vector<KeyFrame>> keyframes;
	std::ifstream in(file_path);

	if (!in.is_open()) {
		printf("\nFailed to open keyframe file");
		return keyframes;
	}

	std::vector<KeyFrame> currentAnim;
	std::string line;

	while (std::getline(in, line)) {
		// Trim whitespace
		if (line.empty()) {
			// End of one animation block
			if (!currentAnim.empty()) {
				keyframes.push_back(currentAnim);
				currentAnim.clear();
			}
			continue;
		}

		std::stringstream ss(line);
		float delay = 0.0f;
		ss >> delay;

		if (!ss.fail()) {
			KeyFrame keyframe;
			keyframe.delaySec = delay;
			currentAnim.push_back(keyframe);
		}
	}

	// Push last animation if file didn’t end with an empty line
	if (!currentAnim.empty()) {
		keyframes.push_back(currentAnim);
	}

	return keyframes;
}

void AnimationDatabase::LoadAnimDataFromFolder(const std::string& basePathFromResourceFolder,
	int numOfAnimationsInFolder, int numOfTexturesPerAnimation, const std::string& animName) {

	std::vector<AnimationData*> animations;
	std::vector<std::vector<KeyFrame>> keyframes;

	std::string basePath = RESOURCES_FOLDER + basePathFromResourceFolder;

	//try to load .prkf file
	keyframes = LoadKeyframesFromFile((basePath + "/" + animName + ".prkf").c_str());

	animations.resize(numOfAnimationsInFolder);

	for (int i = 0; i < numOfAnimationsInFolder; i++)
	{
		animations[i] = new AnimationData();

		//default one -1 keyframe so it never advances
		if (!keyframes.size() > 0)
		{
			animations[i]->keyframes.resize(1);
			animations[i]->keyframes[0].delaySec = -1.0f;
		}
	}

	int currTextureIndex = 0;
	int currSpriteAnimIndex = 0;




	// Traverse directories and subdirectories
	for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(basePath)) {
		if (dirEntry.is_regular_file()) {
			// Get file path and check if it's a texture (by extension, e.g., .png, .jpg)
			std::string filePath = dirEntry.path().string();
			std::string extension = dirEntry.path().extension().string();


			// Skip files that are not textures or keyframe data
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

			std::replace(filePath.begin(), filePath.end(), '\\', '/');


			Texture2D texture = LoadTexture(filePath.c_str());
			

			// Add the texture to the vector with its name
			if (!animName.empty())
			{
				textureName = textureName.append(animName);
			}

			animations[currSpriteAnimIndex]->textures.push_back({ texture, textureName });

			currTextureIndex++;
			if (currTextureIndex >= numOfTexturesPerAnimation)
			{
				currTextureIndex = 0;
				currSpriteAnimIndex++;
				if (currSpriteAnimIndex >= numOfAnimationsInFolder)
				{
					//cache number of textures + assing keyframes to struct
					for (int i = 0; i < numOfAnimationsInFolder; i++)
					{
						animations[i]->numberOfTextures = animations[i]->textures.size();
						if (keyframes.size() > 0)
						{
							animations[i]->keyframes = keyframes[i];
						}

					}

					//this should only happen in the literal end step of the algorithm and if this is ever reached while the 
					//folder is not completely traversed something is very wrong

					//move from vector to unordered map
					for (AnimationData* animation : animations)
					{
						animData.emplace(std::pair<std::string, AnimationData*>(animName, animation));
					}

					return;
				}
			}
		}
	}

	//algo never reaches this point , always from return above ^
}

void AnimationDatabase::LoadAnimDataFromFolder(const std::string& basePathFromResourceFolder,
	const std::string& animName)
{
	std::string basePath = RESOURCES_FOLDER + basePathFromResourceFolder;

	// Load keyframes if file exists
	std::vector<std::vector<KeyFrame>> keyframes;
	const std::string keyframePath = basePath + "/" + animName + ".prkf";

	if (std::filesystem::exists(keyframePath))
		keyframes = LoadKeyframesFromFile(keyframePath.c_str());

	// Create one animation
	AnimationData* anim = new AnimationData();

	// Default keyframe if none exist
	if (keyframes.empty()) {
		anim->keyframes.resize(1);
		anim->keyframes[0].delaySec = -1.0f;
	}
	else {
		anim->keyframes = keyframes[0];
	}

	// Load all textures from folder
	for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(basePath)) {

		if (!dirEntry.is_regular_file())
			continue;

		std::string extension = dirEntry.path().extension().string();
		if (extension != ".png" && extension != ".jpg" && extension != ".jpeg")
			continue;

		std::string filePath = dirEntry.path().string();
		std::replace(filePath.begin(), filePath.end(), '\\', '/');

		// Create texture name from file (no subfolder logic needed anymore)
		std::string textureName = dirEntry.path().stem().string();
		textureName += "_" + animName;

		// Load texture
		Texture2D texture = LoadTexture(filePath.c_str());

		anim->textures.push_back({ texture, textureName });
	}

	anim->numberOfTextures = anim->textures.size();

	// Store in animation map
	animData.emplace(animName, anim);
}

AnimationData* AnimationDatabase::GetAnimationDataFromName(const std::string& name)
{
	auto it = animData.find(name);
	if (it != animData.end()) {
		return it->second;
	}
	return nullptr; // not found
}
