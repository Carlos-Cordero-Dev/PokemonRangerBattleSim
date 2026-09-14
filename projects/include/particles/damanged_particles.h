#pragma once

#include "particle_system.h"
#include "ranger_top.h"

class DamagedParticles : public ParticleSystem
{
public:
	void step(float dt) override;
	void Reset() override;
	void Draw() override;

	void calc_positions_from_enclosed_points(Coord* enclosedPoints);

private:
	static constexpr float kTravelDistance = 200.0f;
	static constexpr float kTravelTimeSec = 0.3f;
	float elapsedTime = 0.0f;

	std::vector<Vector2> initialPositions;
	std::vector<Vector2> currentPositions;
};

inline void DamagedParticles::step(float dt)
{
	if (initialPositions.empty()) return;

	elapsedTime += dt;
	const float interpolationFactor = elapsedTime / kTravelTimeSec;

	for (size_t i = 0; i < initialPositions.size(); ++i)
	{
		const Vector2 targetPosition = Vector2Add(
			initialPositions[i], Vector2{ 0.0f, -kTravelDistance });

		currentPositions[i] = Vector2Add(
			initialPositions[i],
			Vector2Scale(Vector2Subtract(targetPosition, initialPositions[i]), interpolationFactor));
	}

	if (interpolationFactor >= 1.0f) Reset();
}

inline void DamagedParticles::Reset()
{
	initialPositions.clear();
	currentPositions.clear();
	elapsedTime = 0.0f;
}

inline void DamagedParticles::Draw()
{
	for (const Vector2 particlePosition : currentPositions)
	{
		DrawCircleV(particlePosition, 3.0f, RED);
	}
}

inline void DamagedParticles::calc_positions_from_enclosed_points(Coord* enclosedPoints)
{
	Reset();
	CalculateDestroyedIndicatorParticlePositions(enclosedPoints, initialPositions);
	currentPositions = initialPositions;
}