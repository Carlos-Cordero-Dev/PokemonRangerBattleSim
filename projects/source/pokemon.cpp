
#include "pokemon.h"

Pokemon::Pokemon(const std::vector<SpriteAnimation*>& animations) : EnclosableObject(animations)
{

}

void Pokemon::Update()
{
	WorldObject::Update();

}

void Pokemon::OnEnclosed()
{
	printf("enclosed pokimon\n");
}