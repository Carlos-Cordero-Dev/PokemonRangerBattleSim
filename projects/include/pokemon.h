
#pragma once

#include "sprites.h"
#include "enclosable_object.h"
#include "ranger_top.h"

class Pokemon : public EnclosableObject
{
public:
	Pokemon(const std::vector<SpriteAnimation*>& animations);

	void Draw() override;
	void OnEnclosed() override;
	void OnEnclosedSetCenter(Vector2* newCenter);
	void Update() override;
	void Cleanup() override;

	void AssignStateMachine(StateMachine* stateMachine);
	void SetAnimationState(const std::string& animationMame);

	StateMachine* stateMachine = nullptr;

private:

	int id = 0;

	enum AnimationState
	{
		kIdle = 0,
		kMove = 1,
		kAttack = 2,
	};

	//its assumed it gets changed by the state machine designed for this specific pkmn
	AnimationState animationState;
};