
#pragma once

#include <vector>

class Hitbox; //fd

class GameManager {

public:
	static GameManager& GetInstance() {
		static GameManager instance;
		return instance;
	}

public:
	std::vector<Hitbox*> activeHitboxes;

};