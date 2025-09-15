
#pragma once

#ifdef SWITCH_BUILD
#define RESOURCES_FOLDER "romfs:/resources/"
#else
#define RESOURCES_FOLDER "romfs/resources/"
#endif

constexpr int KFPS = 60;
constexpr float kMaxPolyLength = 900.0f;
constexpr int KMarginToClosePoly = 80;
constexpr int KHealthbarPosX = 10, KHealthbarPosY = 10;
constexpr float KSegmentLengthEpsilon = 0.1f; //used when updating top tail 
constexpr float KYellowTransitionShrinkingFactor = 1.0f;
constexpr float kYellowTransitionShrinkingFrequencySec = 0.01f;
constexpr int MAX_TOP_POINTS = 512;
