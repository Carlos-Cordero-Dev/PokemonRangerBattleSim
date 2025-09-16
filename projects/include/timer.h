
#pragma once

#include <cstdint> //uint64_t
#include "constants.h"
#include "raylib.h"

class Timer
{
public:

	static Timer& Timer::GetInstance() {
		static Timer instance;
		return instance;
	}

	//needs to be called every frame
	void Update();

	float GetDeltaTime() const { return m_deltaTime; };
	float GetGameTime() const { return m_gameTime; };
	int GetFrame() const { return m_gameFrame; };

private:
	Timer();
	~Timer();
	Timer(const Timer&) = delete;
	Timer& operator=(const Timer&) = delete;

private:

	float m_deltaTime = 0.0f;
	double m_lastFrameTime = 0.0;

	double m_gameTime = 0.0;
	uint64_t m_gameFrame = 0;
};

