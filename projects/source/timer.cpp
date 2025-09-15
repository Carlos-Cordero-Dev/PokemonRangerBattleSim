
#include "timer.h"
#include "raylib.h"

Timer::Timer() : m_deltaTime(0.0f), m_lastFrameTime(0.0) {
    m_lastFrameTime = GetTime();
}

Timer::~Timer() {}

void Timer::Update() {

    //disabled cause why would I need to call this every frame
    
    //m_lastFrameTime = m_gameTime;
    //m_gameTime = GetTime();
    //m_deltaTime = GetFrameTime();
    m_gameFrame++;
}
