
#pragma once

#include "raylib.h"

namespace prbs {
	struct Vector2
	{
		int x, y;
	};
};

prbs::Vector2 GetTouch()
{
	int touchx = 0;
	int touchy = 0;

#ifdef SWITCH_BUILD
	if (GetTouchPointCount() != 0)
	{
		touchx = GetTouchX();
		touchy = GetTouchY();
	}
#elif WINDOWS_BUILD
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		touchx = GetMouseX();
		touchy = GetMouseY();
	}
#endif
	return {touchx, touchy};
}
