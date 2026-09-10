
#include "animation_database.h"

#include <string>
#include <algorithm>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cctype>

#include "raylib.h"

#include "constants.h"
#include "sprites.h"
//#include "external/nlohmann/json.hpp"

static void TrimWhitespace(std::string& text)
{
	const auto first = text.find_first_not_of(" \t\r\n");
	if (first == std::string::npos) {
		text.clear();
		return;
	}

	const auto last = text.find_last_not_of(" \t\r\n");
	text = text.substr(first, last - first + 1);
}

static bool ValidateKeyframeTextureIndices(const AnimationData& animation, const std::string& animationName)
{
	for (const KeyFrame& keyframe : animation.keyframes) {
		if (keyframe.textureIndex < 0 || keyframe.textureIndex >= animation.numberOfTextures) {
			printf("\nAnimation '%s' references texture index %d, but only has %d textures",
				animationName.c_str(), keyframe.textureIndex, animation.numberOfTextures);
			return false;
		}
	}

	return true;
}

static void AddMirroredVariant(
	AnimationSet& set,
	const std::string& action,
	FacingDirection source,
	FacingDirection destination)
{
	auto actionIt = set.find(action);
	if (actionIt == set.end())
		return;

	auto sourceIt = actionIt->second.find(source);

	if (sourceIt != actionIt->second.end())
		actionIt->second.emplace(destination, AnimationVariant{ sourceIt->second.data, true });
}

static FacingDirection ParseDirection(const std::string& name)
{
    if (name == "up_left")
        return FacingDirection::UpLeft;

    if (name == "up_right")
        return FacingDirection::UpRight;

    if (name == "down_left")
        return FacingDirection::DownLeft;

    if (name == "down_right")
        return FacingDirection::DownRight;

	//default
    return FacingDirection::DownLeft;
}

AnimationSet AnimationDatabase::LoadAnimationSet(const Json& json)
{
	AnimationSet set;

	if (!json.contains("animations") || !json["animations"].is_object()) {
		printf("\nAnimation manifest has no 'animations' object");
		return set;
	}

	const std::string pokemonId = json.value("id", "");

	for (const auto& [action, directions] : json["animations"].items())
	{
		if (!directions.is_object()) {
			printf("\nAnimation action '%s' must contain a direction object", action.c_str());
			continue;
		}

		for (const auto& [directionName, config] : directions.items())
		{
			const std::optional<FacingDirection> direction = ParseDirection(directionName);
			if (!direction) {
				printf("\nUnknown animation direction '%s' for action '%s'",
					directionName.c_str(), action.c_str());
				continue;
			}

			if (!config.contains("path") || !config.contains("animation")) {
				printf("\nAnimation '%s/%s' requires 'path' and 'animation'",
					action.c_str(), directionName.c_str());
				continue;
			}

			std::string path = config["path"].get<std::string>();
			if (path.rfind("sprites/", 0) != 0) {
				if (pokemonId.empty()) {
					printf("\nAnimation '%s/%s' uses a relative path but the manifest has no 'id'",
						action.c_str(), directionName.c_str());
					continue;
				}

				path = "sprites/" + pokemonId + "/" + path;
			}
			const std::string animationName = config["animation"].get<std::string>();
			AnimationData* animation = GetAnimationDataFromName(animationName);

			if (!animation) {
				LoadAnimDataFromFolder(path, animationName);
				animation = GetAnimationDataFromName(animationName);
			}

			if (!animation) {
				printf("\nFailed to load animation '%s' for '%s/%s'",
					animationName.c_str(), action.c_str(), directionName.c_str());
				continue;
			}

			animation->loop = config.value("loop", true);

			set[action][*direction] = { animation, false };
		}
	}

	for (auto& [action, directions] : set) {
		AddMirroredVariant(set, action, FacingDirection::DownLeft, FacingDirection::DownRight);
		AddMirroredVariant(set, action, FacingDirection::UpLeft, FacingDirection::UpRight);
	}

	return set;
}

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

	while (std::getline(in, line))
	{
		// Empty line = end of animation block
		if (line.empty())
		{
			if (!currentAnim.empty()) {
				keyframes.push_back(std::move(currentAnim));
				currentAnim.clear();
			}
			continue;
		}

		std::stringstream lineStream(line);
		std::string segment;
		std::getline(lineStream, segment, '|');
		TrimWhitespace(segment);

		std::stringstream keyframeStream(segment);
		KeyFrame kf;
		if (!(keyframeStream >> kf.textureIndex >> kf.delaySec)) {
			printf("\nInvalid keyframe in %s: %s", file_path.c_str(), line.c_str());
			continue;
		}

		while (std::getline(lineStream, segment, '|'))
		{
			TrimWhitespace(segment);
			if (segment.empty()) {
				printf("\nEmpty animation event in %s: %s", file_path.c_str(), line.c_str());
				continue;
			}

			std::stringstream eventStream(segment);
			std::string eventName;
			eventStream >> eventName;

			if (eventName == "melee_hitbox") {

				auto hitboxEvent = std::make_unique<HitboxAnimationEvent>();

				if (!(eventStream >> hitboxEvent->offsetX
					>> hitboxEvent->offsetY
					>> hitboxEvent->width
					>> hitboxEvent->height)) {
					printf("\nInvalid melee_hitbox event in %s: %s",
						file_path.c_str(), line.c_str());
					continue;
				}

				kf.events.push_back(std::move(hitboxEvent));

			}
			else
			{
				printf("\nUnknown animation event '%s' in %s", eventName.c_str(), file_path.c_str());
			}
		}

		currentAnim.push_back(std::move(kf));
	}

	// Push last animation if file does not end with blank line
	if (!currentAnim.empty()) {
		keyframes.push_back(std::move(currentAnim));
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
							animations[i]->keyframes = std::move(keyframes[i]);
						}
						ValidateKeyframeTextureIndices(*animations[i], animName);

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
	if (GetAnimationDataFromName(animName))
		return;

	std::string basePath = RESOURCES_FOLDER + basePathFromResourceFolder;

	if (!std::filesystem::exists(basePath)) {
		printf("\nAnimation folder does not exist: %s", basePath.c_str());
		return;
	}

	// Load keyframes if file exists
	std::vector<std::vector<KeyFrame>> keyframes;
	const std::string keyframePath = basePath + "/" + animName + ".prkf";

	if (std::filesystem::exists(keyframePath))
		keyframes = LoadKeyframesFromFile(keyframePath.c_str());

	// Create one animation
	AnimationData* anim = new AnimationData();
	anim->name = animName;

	// Default keyframe if none exist
	if (keyframes.empty()) {
		anim->keyframes.resize(1);
		anim->keyframes[0].delaySec = -1.0f;
	}
	else {
		anim->keyframes = std::move(keyframes[0]);
	}

	// Load all textures from folder
	std::vector<std::filesystem::path> texturePaths;
	for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(basePath)) {

		if (!dirEntry.is_regular_file())
			continue;

		std::string extension = dirEntry.path().extension().string();
		if (extension != ".png" && extension != ".jpg" && extension != ".jpeg")
			continue;

		texturePaths.push_back(dirEntry.path());
	}

	std::sort(texturePaths.begin(), texturePaths.end(),
		[](const std::filesystem::path& left, const std::filesystem::path& right) {
			const std::string leftStem = left.stem().string();
			const std::string rightStem = right.stem().string();
			const bool leftIsNumber = !leftStem.empty() && std::all_of(
				leftStem.begin(), leftStem.end(), [](unsigned char c) { return std::isdigit(c); });
			const bool rightIsNumber = !rightStem.empty() && std::all_of(
				rightStem.begin(), rightStem.end(), [](unsigned char c) { return std::isdigit(c); });

		if (leftIsNumber && rightIsNumber && leftStem.size() != rightStem.size())
			return leftStem.size() < rightStem.size();

		return leftStem < rightStem;
	});

	for (const std::filesystem::path& texturePath : texturePaths) {

		std::string filePath = texturePath.string();
		std::replace(filePath.begin(), filePath.end(), '\\', '/');

		// Create texture name from file (no subfolder logic needed anymore)
		std::string textureName = texturePath.stem().string();
		textureName += "_" + animName;

		// Load texture
		Texture2D texture = LoadTexture(filePath.c_str());

		anim->textures.push_back({ texture, textureName });
	}

	anim->numberOfTextures = anim->textures.size();
	anim->totalFrames = anim->keyframes.size();
	if (anim->textures.empty()) {
		printf("\nAnimation '%s' has no textures in %s", animName.c_str(), basePath.c_str());
		delete anim;
		return;
	}

	ValidateKeyframeTextureIndices(*anim, animName);

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
