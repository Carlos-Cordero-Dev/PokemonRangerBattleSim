
#include "timer.h"
#include "raylib.h"

Timer::Timer() : m_deltaTime(0.0f), m_lastFrameTime(0.0) {
    m_gameTime = 0.0;
    m_lastFrameTime = m_gameTime;
    m_timeScale = 1.0f;
}

Timer::~Timer() {}

void Timer::Update() {
    
    m_lastFrameTime = m_gameTime;
    m_deltaTime = GetFrameTime() * m_timeScale;
    m_gameTime += m_deltaTime;
    m_gameFrame++;
}
