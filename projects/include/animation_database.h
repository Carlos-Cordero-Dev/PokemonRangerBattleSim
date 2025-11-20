
#pragma once

#include <string>
#include <vector>
#include <unordered_map>

struct TextureData;

struct KeyFrame
{
	float delaySec = 0.0f; //time to spend in this frame
};

struct AnimationData
{
	std::vector<TextureData> textures;
	std::vector<KeyFrame> keyframes;

	std::string name;
	int id = 0;
	int numberOfTextures = 0;
};

class AnimationDatabase
{
public:
	AnimationDatabase() {};

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