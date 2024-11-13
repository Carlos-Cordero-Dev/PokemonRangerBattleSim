
#ifndef TIMER_H_
#define TIMER_H_ 1

#include <chrono>
#include "constants.h"

typedef std::chrono::high_resolution_clock GClock;

typedef GClock::time_point GTimePoint;
typedef std::chrono::duration<uint64_t, std::ratio<1, 1000>> GTimeUnit;
typedef std::chrono::duration<double, std::ratio<1, 1>> GImpulseUnit;
typedef std::chrono::duration<double, std::ratio<1, KFPS>> GMaxFPSUnit; //Max FPS, ratio<1,60> = 60 FPS

class Timer
{
public:
	//HAS TO BE REFACTORED INTO NOT BEING ABLE TO BE INSTANCIATED MULTIPLE TIMES
	Timer();
	~Timer(){};

	[[nodiscard]] inline double get_delta_time() { return (double)deltaTime_.count(); };	
	[[nodiscard]] inline int get_time_ms() { return (int)gameTime_.count(); };	//milliseconds

	void Update();
	void Reset();
	void FrameSleep();

	uint64_t frame = 0;
private:

	Timer& operator=(const Timer&) = delete;

private:


	const GMaxFPSUnit GMaxFPS_{ 1 };

	GTimeUnit gameTime_;
	GImpulseUnit deltaTime_;

	GTimePoint gameStart_;
	GTimePoint gameFrameBegin_;
	GTimePoint gameFrameEnd_;
	GTimePoint gameFrameMark_;
};
#endif
