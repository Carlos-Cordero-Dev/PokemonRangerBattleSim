
#pragma once

#ifdef SWITCH_BUILD
#define RESOURCES_FOLDER "romfs:/resources/"
#else
#define RESOURCES_FOLDER "romfs/resources/"
#endif

constexpr int KFPS = 60;
constexpr int KMarginToClosePoly = 80;
constexpr int KHealthbarPosX = 10, KHealthbarPosY = 10;
constexpr int MAX_TOP_POINTS = 512; //I want the texture to be a power of 2 even tho it has no mipmaps
