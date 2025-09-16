
#include "timer.h"
#include "raylib.h"

Timer::Timer() : m_deltaTime(0.0f), m_lastFrameTime(0.0) {
    m_gameTime = GetTime();
    m_lastFrameTime = m_gameTime;
}

Timer::~Timer() {}

void Timer::Update() {
    
    m_lastFrameTime = m_gameTime;
    m_gameTime = GetTime();
    m_deltaTime = (float)(m_gameTime - m_lastFrameTime);
    m_gameFrame++;
}
