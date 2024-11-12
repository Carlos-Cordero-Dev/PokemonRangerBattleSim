
#pragma once

#include "raylib.h"
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
// NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)

class Sprite {

	Sprite()
	{

	}
};

struct TextureData {
	Texture2D texture;
	std::string name;
};

std::vector<TextureData> textures;

void loadTexturesFromFolder(const std::string& basePath) {

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
			textures.push_back({ texture, textureName });
		}
	}
	int i = 1;
	if (i == 1);

}
 