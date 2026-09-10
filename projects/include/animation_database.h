
#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "json_loader.h"

struct TextureData;

enum class AnimationEventType {
	MeleeHitbox
};

struct AnimationEvent {
	AnimationEventType type;
	std::string eventName; //unused
};

struct HitboxAnimationEvent : AnimationEvent {
	HitboxAnimationEvent() : AnimationEvent() {
		type = AnimationEventType::MeleeHitbox;
	}
	//note: these should never be negative values
	float offsetX = 0.0f;
	float offsetY = 0.0f;
	float height = 0.0f;
	float width = 0.0f;
};

struct KeyFrame
{
	int textureIndex = 0;//TODO: index should match the name of the texture: ex pikachu_down_0 should be 0
	float delaySec = 0.0f; //time to spend in this frame
	std::vector<AnimationEvent> events;
};

struct AnimationData
{
	std::vector<TextureData> textures;
	std::vector<KeyFrame> keyframes;

	std::string name;
	//int id = 0; //unused
	int numberOfTextures = 0;// as many textures as an animation has
	int totalFrames = 0; //includes repeating indices so 0 1 2 1 2 is 5 frames
	bool loop = true;
};

struct AnimationVariant
{
	AnimationData* data = nullptr;
	bool flipX = false;
};

enum FacingDirection {
	UpLeft = 0,
	UpRight,
	DownLeft,
	DownRight
};

using DirectionalAnimations =
std::unordered_map<FacingDirection, AnimationVariant>;

using AnimationSet =
std::unordered_map<std::string, DirectionalAnimations>;

class AnimationDatabase
{
public:

	static AnimationDatabase& Instance() {
		static AnimationDatabase instance;
		return instance;
	}

	AnimationSet LoadAnimationSet(const Json& json);


	//animName must match .prkf name file for propper keyframe data loading
	void LoadAnimDataFromFolder(const std::string& basePathFromResourceFolder,
		const std::string& animName);

	//NOTE: Legacy version
	//loads from path, where 4,6  would be 4 animations with 6 frames each 
	//animName is a custom, unique string that identfies a set of frames
	//note: animName must match .prkf name file for propper keyframe data loading
	void LoadAnimDataFromFolder(const std::string& basePathFromResourceFolder,
		int numOfAnimationsInFolder, int numOfTexturesPerAnimation, const std::string& animName);

	AnimationData* GetAnimationDataFromName(const std::string& name);
	
	//TODO: destructor which frees all the animations stored

private:
	std::unordered_map<std::string,AnimationData*> animData;
};