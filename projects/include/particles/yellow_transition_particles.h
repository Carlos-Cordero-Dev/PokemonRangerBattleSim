#pragma once

#include "particle_system.h"
#include "top.h"

class YellowTransitionParticles : public ParticleSystem
{
public:
	void step(float dt) override;
	void Reset() override;
	void Draw() override;
	void on_intersection(Coord* enclosedPolygon);

private:
	Coord* ownedEnclosedPolygon = nullptr;
	std::vector<Vector2> areaPoints;
	float elapsedTime = 0.0f;
	float currentFadeFactor = 1.0f;

	static constexpr float KYellowTransitionShrinkingFactor = 0.25f;
	static constexpr float kYellowTransitionShrinkingFrequencySec = 0.005f;
	static constexpr float kInitialThickness = 30.0f;

	static constexpr float kInitialFadeFactor = 0.75f;
	static constexpr float kFadeSpeed = 0.0005f;
};

inline void YellowTransitionParticles::step(float dt)
{
	elapsedTime += dt;

	if (elapsedTime <= kYellowTransitionShrinkingFrequencySec) return;

	elapsedTime = 0.0f;
	for (size_t i = 0; i + 3 < areaPoints.size(); i += 4)
	{
		const Vector2 upDirection = Vector2Normalize(
			Vector2Subtract(areaPoints[i + 2], areaPoints[i]));

		areaPoints[i].y		+= upDirection.y * KYellowTransitionShrinkingFactor;
		areaPoints[i + 1].y += upDirection.y * KYellowTransitionShrinkingFactor;
		areaPoints[i + 2].y -= upDirection.y * KYellowTransitionShrinkingFactor;
		areaPoints[i + 3].y -= upDirection.y * KYellowTransitionShrinkingFactor;
	}
}

inline void YellowTransitionParticles::Reset()
{
	DestroyStackNoDepth(&ownedEnclosedPolygon);
	areaPoints.clear();
	elapsedTime = 0.0f;
	currentFadeFactor = 1.0f;
}

inline void YellowTransitionParticles::Draw()
{

	Color particleColor = { 255, 255, 0, 255 };
	particleColor.a = static_cast<unsigned char>(255.0f * currentFadeFactor);
	currentFadeFactor -= kFadeSpeed;
	if (currentFadeFactor < 0.0f) currentFadeFactor = 0.0f;

	for (size_t i = 0; i + 3 < areaPoints.size(); i += 4)
	{
		const Vector2 topLeft = areaPoints[i];
		const Vector2 topRight = areaPoints[i + 1];
		const Vector2 bottomLeft = areaPoints[i + 2];
		const Vector2 bottomRight = areaPoints[i + 3];

		DrawTriangle(topLeft, bottomLeft, topRight, particleColor);
		DrawTriangle(topLeft, topRight, bottomLeft, particleColor);
		DrawTriangle(topRight, bottomLeft, bottomRight, particleColor);
		DrawTriangle(topRight, bottomRight, bottomLeft, particleColor);
	}
}

static void CalculateEnclosedShaderAreaPoints(
	Coord* enclosedPoints, std::vector<Vector2>& vectorToFill, float thickness)
{
	const float halfThickness = thickness * 0.5f;

	Coord* aux = enclosedPoints;
	Coord* auxNext = enclosedPoints->nextCoord;
	bool completed = false;

	while (!completed)
	{
		if (aux->nextCoord == nullptr)
		{
			//circle back to begginig
			auxNext = enclosedPoints;
			completed = true;
		}

		Vector2 topLeft = { aux->x,aux->y + halfThickness };
		Vector2 topRight = { auxNext->x,auxNext->y + halfThickness };
		Vector2 botLeft = { aux->x,aux->y - halfThickness };
		Vector2 botRight = { auxNext->x,auxNext->y - halfThickness };

		vectorToFill.emplace_back(topLeft);
		vectorToFill.emplace_back(topRight);
		vectorToFill.emplace_back(botLeft);
		vectorToFill.emplace_back(botRight);

		aux = auxNext;
		auxNext = auxNext->nextCoord;
	}
}

inline void YellowTransitionParticles::on_intersection(Coord* enclosedPolygon)
{
	Reset();
	ownedEnclosedPolygon = enclosedPolygon;
	CalculateEnclosedShaderAreaPoints(ownedEnclosedPolygon, areaPoints, kInitialThickness);
}