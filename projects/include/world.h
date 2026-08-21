
#pragma once

#include <vector>

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

};