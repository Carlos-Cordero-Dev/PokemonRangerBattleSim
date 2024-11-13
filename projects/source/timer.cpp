
#include "timer.h"

#include <chrono>
#include <thread>

#define GNow() GClock::now()
#define GTimeCast(value) std::chrono::duration_cast<GTimeUnit>(value)
#define GImpCast(value) std::chrono::duration_cast<GImpulseUnit>(value)

Timer::Timer()
{
	this->Reset();
}

void Timer::Reset()
{
	gameStart_ = GNow();
	gameFrameBegin_ = gameStart_;
}

void Timer::Update()
{
	auto currentTime = GNow();
	gameTime_ = GTimeCast(currentTime - gameStart_);
	deltaTime_ = GImpCast(currentTime - gameFrameBegin_);
	gameFrameBegin_ = currentTime;
	frame++;
}

void Timer::FrameSleep()
{
	gameFrameEnd_ = GNow();
	gameFrameMark_ = gameFrameBegin_;

	GImpulseUnit frameImpulse = (gameFrameEnd_ - gameFrameBegin_);
	if (frameImpulse < GMaxFPS_) { //TOO FAST, Limit the framerate by sleeping.
		std::this_thread::sleep_for(GMaxFPS_ - frameImpulse);
	}
}