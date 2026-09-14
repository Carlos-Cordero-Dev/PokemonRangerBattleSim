#pragma once

#include "particle_system.h"
#include "ranger_top.h"

class EnclosingParticles : public ParticleSystem
{
public:
	void step(float dt) override;
	void Reset() override;
	void Draw() override;

	void on_enclosed_pokemon(const Vector2& enclosedCenter, Coord* enclosedPolygon);

private:
	static constexpr float kTravelTimeSec = 0.3f;
	float elapsedTime = 0.0f;
	Vector2 trackingPosition = { -100.0f, -100.0f };

	std::vector<Vector2> initialPositions;
	std::vector<Vector2> currentPositions;
};

inline void EnclosingParticles::step(float dt)
{
	if (initialPositions.empty()) return;

	elapsedTime += dt;
	const float interpolationFactor = elapsedTime / kTravelTimeSec;

	for (size_t i = 0; i < initialPositions.size(); ++i)
	{
		currentPositions[i] = Vector2Add(
			initialPositions[i],
			Vector2Scale(Vector2Subtract(trackingPosition, initialPositions[i]), interpolationFactor));
	}

	if (interpolationFactor >= 1.0f) Reset();
}

inline void EnclosingParticles::Reset()
{
	initialPositions.clear();
	currentPositions.clear();
	trackingPosition = { -100.0f, -100.0f };
	elapsedTime = 0.0f;
}

inline void EnclosingParticles::Draw()
{
	for (const Vector2 particlePosition : currentPositions)
	{
		DrawCircleV(particlePosition, 3.0f, RED);
	}
}

inline void EnclosingParticles::on_enclosed_pokemon(
	const Vector2& enclosedCenter, Coord* enclosedPolygon)
{
	Reset();
	trackingPosition = enclosedCenter;
	CalculateClosingIndicatorParticlePoints(enclosedPolygon, initialPositions);
	currentPositions = initialPositions;
}