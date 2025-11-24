
#include <vector>

class Hitbox;

class World {
public:
	std::vector<Hitbox*> activeHitboxes;

};

extern World* g_world; 