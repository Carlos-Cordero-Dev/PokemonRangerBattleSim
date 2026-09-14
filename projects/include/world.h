
#pragma once

#include <vector>
#include <raylib.h> //Rectangle

class Hitbox; //fd
class HazardObject;
class WorldObject;
class EnclosableObject;


class GameManager {

public:
	static GameManager& GetInstance() {
		static GameManager instance;
		return instance;
	}

public:
	std::vector<Hitbox*> activeHitboxes;
	std::vector<HazardObject*> activeHazardObjects;
	std::vector<WorldObject*> allWorldObjs;
	std::vector<EnclosableObject*> allEnclosableObjs;
	WorldObject* topObjectPtr = nullptr;
	Rectangle playableArea = {0.0f,0.0f,0.0f,0.0f};

	std::vector<Hitbox*> singleFrameHitboxes;
	std::vector<Hitbox*> singleKeyframeHitboxes;
};