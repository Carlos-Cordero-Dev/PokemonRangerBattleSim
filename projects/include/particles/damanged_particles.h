#pragma once

#include "particle_system.h"
#include "top.h"

class DamagedParticles : public ParticleSystem
{
public:
	DamagedParticles();

	void step(float dt) override;
	void Reset() override;
	void Draw() override;

	void calc_positions_from_enclosed_points(Coord* enclosedPoints);

private:

	static constexpr float kDefaultTravelSpeed = 300.0f;
	static constexpr float kMinStartAcceleration = 1.0f;
	static constexpr float kMaxStartAcceleration = 4.0f;
	static constexpr float kLifetime = 4.0f;
	static constexpr float kFadeFactor = 0.08f;
	static constexpr float kStartFade = 1.0f;

	float elapsedTime = 0.0f;
	float currAlpha = 1.0f;

	std::vector<Vector2> initialPositions;
	std::vector<Vector2> currentPositions;
	std::vector<float> initialAccelerations;
	std::vector<float> currAccelerations;

};

DamagedParticles::DamagedParticles()
{
	Reset();
}

inline void DamagedParticles::step(float dt)
{
	if (initialPositions.empty()) return;


	for (int i = 0; i < initialPositions.size(); ++i)
	{
		currAccelerations[i] += initialAccelerations[i] * dt;

		float distanceToTravel = kDefaultTravelSpeed * currAccelerations[i] * dt;
		currentPositions[i].y -= distanceToTravel;
	}

	elapsedTime += dt;
	currAlpha = std::max<float>(0.0f, currAlpha - (kFadeFactor * dt));
	if (elapsedTime >= kLifetime) Reset();
}

inline void DamagedParticles::Reset()
{
	initialPositions.clear();
	currentPositions.clear();
	currAccelerations.clear();
	initialAccelerations.clear();
	elapsedTime = 0.0f;
	currAlpha = kStartFade;
}

inline void DamagedParticles::Draw()
{
	const Color particleColor = Fade(WHITE, currAlpha);

	for (const Vector2 particlePosition : currentPositions)
	{
		DrawCircleV(particlePosition, 3.0f, particleColor);
	}
}

static void CalculateDestroyedIndicatorParticlePositions(Coord* enclosedPoints, std::vector<Vector2>& outParticlePositions)
{
	constexpr int kMaxParticleCount = 20;
	Coord* aux = enclosedPoints;
	float enclosedTotalLength = 0.0f;
	//calculate length

	Vector2 particlePos;

	while (aux && aux->nextCoord)
	{
		Vector2 currentPos = { aux->x,aux->y };
		Vector2 nextPos = { aux->nextCoord->x,aux->nextCoord->y };
		particlePos = currentPos;

		outParticlePositions.push_back(particlePos);

		aux = aux->nextCoord;
	}
	//printf("\nlength %f", enclosedTotalLength);
}

inline void DamagedParticles::calc_positions_from_enclosed_points(Coord* enclosedPoints)
{
	Reset();
	CalculateDestroyedIndicatorParticlePositions(enclosedPoints, initialPositions);
	currentPositions = initialPositions;
	currAccelerations.assign(initialPositions.size(), 1.0f);
	initialAccelerations.assign(initialPositions.size(), 1.0f);

	for (int i = 0; i < initialPositions.size(); ++i)
	{
		initialAccelerations[i] = GetRandomValue(
			static_cast<int>(kMinStartAcceleration * 100),
			static_cast<int>(kMaxStartAcceleration * 100)) / 100.0f;
	}
}