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
	void on_trail_break(Coord* trail);

private:
	Coord* ownedEnclosedPolygon = nullptr;
	std::vector<Vector2> areaPoints;

	float currentFadeFactor = 1.0f;

	static constexpr float kYellowTransitionShrinkingSpeed = 50.0f;
	static constexpr float kInitialThickness = 30.0f;

	static constexpr float kInitialFadeFactor = 0.95f;
	static constexpr float kFadeSpeed = 0.25f;
};

inline void YellowTransitionParticles::step(float dt)
{
	if (areaPoints.empty()) return;

	const float shrinkDistance = kYellowTransitionShrinkingSpeed * dt;
	bool fullyCollapsed = true;

	for (size_t i = 0; i + 3 < areaPoints.size(); i += 4)
	{
		const Vector2 upDirection = Vector2Normalize(
			Vector2Subtract(areaPoints[i + 2], areaPoints[i]));

		const float halfRemainingThickness =
			Vector2Distance(areaPoints[i], areaPoints[i + 2]) * 0.5f;
		const float stepDistance = std::min<float>(shrinkDistance, halfRemainingThickness);

		areaPoints[i].y += upDirection.y * stepDistance;
		areaPoints[i + 1].y += upDirection.y * stepDistance;
		areaPoints[i + 2].y -= upDirection.y * stepDistance;
		areaPoints[i + 3].y -= upDirection.y * stepDistance;

		if (halfRemainingThickness > shrinkDistance) fullyCollapsed = false;
	}

	currentFadeFactor = std::max<float>(0.0f, currentFadeFactor - kFadeSpeed * dt);
	if (fullyCollapsed || currentFadeFactor <= 0.0f) Reset();
}

inline void YellowTransitionParticles::Reset()
{
	DestroyStackNoDepth(&ownedEnclosedPolygon);
	areaPoints.clear();
	currentFadeFactor = kInitialFadeFactor;
}

inline void YellowTransitionParticles::Draw()
{

	Color particleColor = { 255, 255, 0, 255 };
	particleColor.a = static_cast<unsigned char>(255.0f * currentFadeFactor);

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
	Coord* enclosedPoints, std::vector<Vector2>& vectorToFill, float thickness, bool closeLoop)
{
	const float halfThickness = thickness * 0.5f;

	Coord* aux = enclosedPoints;
	while (aux && aux->nextCoord)
	{
		Coord* auxNext = aux->nextCoord;

		Vector2 topLeft = { aux->x,aux->y + halfThickness };
		Vector2 topRight = { auxNext->x,auxNext->y + halfThickness };
		Vector2 botLeft = { aux->x,aux->y - halfThickness };
		Vector2 botRight = { auxNext->x,auxNext->y - halfThickness };

		vectorToFill.emplace_back(topLeft);
		vectorToFill.emplace_back(topRight);
		vectorToFill.emplace_back(botLeft);
		vectorToFill.emplace_back(botRight);

		aux = auxNext;
	}

	if (closeLoop && aux && aux != enclosedPoints)
	{
		vectorToFill.emplace_back(Vector2{ aux->x, aux->y + halfThickness });
		vectorToFill.emplace_back(Vector2{ enclosedPoints->x, enclosedPoints->y + halfThickness });
		vectorToFill.emplace_back(Vector2{ aux->x, aux->y - halfThickness });
		vectorToFill.emplace_back(Vector2{ enclosedPoints->x, enclosedPoints->y - halfThickness });
	}
}

inline void YellowTransitionParticles::on_intersection(Coord* enclosedPolygon)
{
	Reset();
	ownedEnclosedPolygon = enclosedPolygon;
	CalculateEnclosedShaderAreaPoints(ownedEnclosedPolygon, areaPoints, kInitialThickness, true);
}

inline void YellowTransitionParticles::on_trail_break(Coord* trail)
{
	Reset();
	CalculateEnclosedShaderAreaPoints(trail, areaPoints, kInitialThickness, false);
}