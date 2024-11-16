
#pragma once

#ifdef SWITCH_BUILD
#define RESOURCES_FOLDER "romfs:/resources/"
#else
#define RESOURCES_FOLDER "romfs/resources/"
#endif

constexpr int KFPS = 60;
constexpr int KMarginToClosePoly = 40;
constexpr int KHealthbarPosX = 10, KHealthbarPosY = 10;