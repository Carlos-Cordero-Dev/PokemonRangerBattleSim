
#pragma once

#include <string>

#include "raylib.h"

#include "sprites.h"


class WorldObject; // fd

class VisualEffect {
public:
	VisualEffect(
		AnimationData* animationData,
		WorldObject* owner,
		const std::string& animationName,
		Vector2 offset,
		float scale,
		float rotationDeg,
		bool flipX,
		bool followOwner,
		bool drawBehindOwner);

	void Update();
	void Draw();
	bool IsFinished() const { return animation.IsFinished(); }
	bool DrawsBehindOwner() const { return drawBehindOwner; }

private:
	SpriteAnimation animation;
	WorldObject* owner = nullptr;
	std::string animationName;
	Vector2 position = { 0.0f, 0.0f };
	Vector2 offset = { 0.0f, 0.0f };
	float scale = 1.0f;
	float rotationDeg = 0.0f;
	bool flipX = false;
	bool followOwner = true;
	bool drawBehindOwner = true;
};