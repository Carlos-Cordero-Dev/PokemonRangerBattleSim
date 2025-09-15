
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

	void LoadAnimDataFromFolder(const std::string& basePathFromResourceFolder,
		int numOfAnimationsInFolder, int numOfTexturesPerAnimation, const std::string& texNamePrefix);

	AnimationData* GetAnimationDataFromName(const std::string& name);
	//TODO: destructor which liberates all the animations stored
private:
	std::unordered_map<std::string,AnimationData*> animData;
};