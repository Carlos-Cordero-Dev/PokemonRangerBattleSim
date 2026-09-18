#pragma once

#include "particle_system.h"
#include "top.h"

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
		DrawCircleV(particlePosition, 3.0f, WHITE);
	}
}

static void CalculateClosingIndicatorParticlePoints(Coord* enclosedPoints, std::vector<Vector2>& outParticlePositions)
{
	constexpr int kMaxParticleCount = 20;


	Coord* aux = enclosedPoints;

	float enclosedTotalLength = 0.0f;

	//calculate length
	while (aux && aux->nextCoord)
	{
		enclosedTotalLength += Dist(aux, aux->nextCoord);

		aux = aux->nextCoord;
	}
	//printf("\nlength %f", enclosedTotalLength);


	//default distribution at (TODO : ??? > 20 maybe idk help me)
	float defaultStep = enclosedTotalLength / kMaxParticleCount;
	aux = enclosedPoints;

	float currLength = 0.0f;
	float currStep = defaultStep;
	float leftoverLength = 0.0f; //small overflow in length should mean the point is generated where the overflow lands

	while (aux && aux->nextCoord)
	{
		currLength += Dist(aux, aux->nextCoord);
		while (currStep < currLength)
		{
			int i = 1;
			Vector2 currentPos = { aux->x,aux->y };
			Vector2 nextPos = { aux->nextCoord->x,aux->nextCoord->y };
			Vector2 dir = Vector2Normalize(Vector2Subtract(nextPos, currentPos));

			Vector2 particlePos;
			//quick hack to only account for leftover overflow the first time in each segment
			if (leftoverLength > 0.0f)
			{
				particlePos = Vector2Add(currentPos, Vector2Scale(dir, leftoverLength));

			}
			else
			{

				particlePos = Vector2Add(currentPos, Vector2Scale(dir, defaultStep * i));
				i++;
			}

			outParticlePositions.push_back(particlePos);

			currStep += defaultStep;
		}
		leftoverLength = currStep - currLength;

		aux = aux->nextCoord;
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