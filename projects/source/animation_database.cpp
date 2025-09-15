
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
	int numOfAnimationsInFolder, int numOfTexturesPerAnimation, const std::string& texNamePrefix) {

	std::vector<AnimationData*> animations;
	std::vector<std::vector<KeyFrame>> keyframes;

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
			if (extension != ".png" && extension != ".jpg" && extension != ".jpeg" && extension != ".prkf"/*keyframe extension*/) {
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

			if (extension == ".prkf" && category.compare(texNamePrefix) == 0/*name of prkf must match parameter prefix*/)
			{
				//keyframe file for the whole folder
				keyframes = LoadKeyframesFromFile(filePath.c_str());
				continue;
			}

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
