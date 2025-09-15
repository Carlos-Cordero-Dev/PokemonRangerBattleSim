
#pragma once

#include "sprites.h"
#include "enclosable_object.h"

class Pokemon : public EnclosableObject
{
public:
	Pokemon(const std::vector<SpriteAnimation*>& animations);

	void OnEnclosed() override;
	void Update();
	void Cleanup();

public:
	int id = 0;

	// IA COMPONENT STATE MACHINE EXTERNAL TOOL
	// IA* stateMachine;


};