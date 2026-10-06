#pragma once

#include "particle_system.h"
#include "top.h"

#include <cmath>

class YellowTransitionParticles : public ParticleSystem
{
public:
	void step(float dt) override;
	void Reset() override;
	void Draw() override;
	void Draw(float pixelSize);
	void on_intersection(Coord* enclosedPolygon);
	void on_trail_break(Coord* trail);
	float GetFadeFactor() const { return currentFadeFactor; }

private:
	Coord* ownedEnclosedPolygon = nullptr;
	std::vector<Vector2> areaPoints;

	float currentFadeFactor = 1.0f;

	static constexpr float kYellowTransitionShrinkingSpeed = 50.0f;
	static constexpr float kInitialThickness = 30.0f;

	static constexpr float kInitialFadeFactor = 0.85f;
	static constexpr float kFadeSpeed = 2.525f;
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
	Draw(1.0f);
}

inline void YellowTransitionParticles::Draw(float pixelSize)
{
	const Color particleColor = { 255, 255, 0, 255 };
	const auto snapToPixelCenter = [pixelSize](Vector2 point) {
		return Vector2{
			(std::floor(point.x / pixelSize) + 0.5f) * pixelSize,
			(std::floor(point.y / pixelSize) + 0.5f) * pixelSize
		};
		};

	for (size_t i = 0; i + 3 < areaPoints.size(); i += 4)
	{
		const Vector2 topLeft = areaPoints[i];
		const Vector2 topRight = areaPoints[i + 1];
		const Vector2 bottomLeft = areaPoints[i + 2];
		const Vector2 bottomRight = areaPoints[i + 3];
		if (topLeft.y == bottomLeft.y && topRight.y == bottomRight.y) continue;

		DrawTriangle(topLeft, bottomLeft, topRight, particleColor);
		DrawTriangle(topLeft, topRight, bottomLeft, particleColor);
		DrawTriangle(topRight, bottomLeft, bottomRight, particleColor);
		DrawTriangle(topRight, bottomRight, bottomLeft, particleColor);

		const Vector2 centerStart = snapToPixelCenter(Vector2Scale(Vector2Add(topLeft, bottomLeft), 0.5f));
		const Vector2 centerEnd = snapToPixelCenter(Vector2Scale(Vector2Add(topRight, bottomRight), 0.5f));
		if (centerStart.x != centerEnd.x || centerStart.y != centerEnd.y)
			DrawLineEx(centerStart, centerEnd, pixelSize, particleColor);
		DrawCircleV(centerStart, pixelSize * 0.5f, particleColor);
		DrawCircleV(centerEnd, pixelSize * 0.5f, particleColor);
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