
#pragma once

#ifdef SWITCH_BUILD
#define RESOURCES_FOLDER "romfs:/resources/"
#else
#define RESOURCES_FOLDER "romfs/resources/"
#endif

constexpr int KFPS = 60;