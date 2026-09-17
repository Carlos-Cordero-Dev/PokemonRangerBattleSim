
#include "world.h"
#include "world_object.h"

void GameManager::RemoveInactiveWorldObjects() {
	for (auto it = allWorldObjs.begin();
		it != allWorldObjs.end();)
	{
		WorldObject* object = *it;

		if (object->pendingDestroy) {
			object->Cleanup();
			delete object;
			it = allWorldObjs.erase(it);
		}
		else {
			++it;
		}
	}
}