
#pragma once

#include "sprites.h"
#include "enclosable_object.h"
#include "ranger_top.h"
#include "particle_system.h"

class Pokemon : public EnclosableObject
{
public:
	Pokemon(const std::vector<SpriteAnimation*>& animations);

	void Draw() override;
	void OnEnclosed() override;
	void OnEnclosedEx(Vector2* newCenter, float incomingDamage);
	void Update() override;
	void Cleanup() override;

	void AssignStateMachine(StateMachine* stateMachine);
	void SetAnimationState(const std::string& animationMame);

	StateMachine* stateMachine = nullptr;

	enum AnimationState
	{
		kIdle = 0,
		kMove = 1,
		kAttack = 2,
	};
	//its assumed it gets changed by the state machine designed for this specific pkmn
	AnimationState animationState;
	
	ParticleSystem* particleSystem = nullptr;
	bool isEnclosed = false; //lasts for enclosedTimer seconds after OnEnclosed is called, then it goes back to false
	float currHealth = 100.0f;
	float maxHealth = 100.0f;

private:
	int id = 0;
};