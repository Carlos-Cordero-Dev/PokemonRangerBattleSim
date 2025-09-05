
#include "pokemon.h"

Pokemon::Pokemon(SpriteAnimation** sa) : EnclosableObject(sa)
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