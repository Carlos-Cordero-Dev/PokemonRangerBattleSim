
#pragma once

#include "animation_database.h"
#include "sprites.h"
#include "enclosable_object.h"
#include "top.h"
#include "particles/particle_system.h"

class Pokemon : public EnclosableObject
{
public:
	explicit Pokemon(const AnimationSet& animations);

	void Draw() override;
	void OnEnclosed() override;
	void OnEnclosedEx(Vector2* newCenter, float incomingDamage);
	void Update() override;
	void Cleanup() override;

	void AssignStateMachine(StateMachine* stateMachine);
	void SetAnimationState(const std::string& animationName);
	void SetFacingDirection(FacingDirection direction);
	bool IsAnimationFinished() const;
	const AnimationVariant* ResolveAnimation() const;
	const AnimationVariant* ResolveAnimation(const std::string& animationName) const;

	StateMachine* stateMachine = nullptr;

	ParticleSystem* particleSystem = nullptr;
	bool isEnclosed = false; //lasts for enclosedTimer seconds after OnEnclosed is called, then it goes back to false
	float currHealth = 100.0f;
	float maxHealth = 100.0f;

	SpriteAnimation animationPlayer;
	AnimationSet animationSet;
	FacingDirection facingDir;
	float shadowScale = 1.0f;

	// must match one of the animation names in the state machine
	std::string animationState;

	//same warning sign sprite for every pokemon
	static inline SpriteAnimation* warningSignSprite;
	bool drawWariningSign = false;
	Vector2 warningSignOffset = { 0.0f, 0.0f };

private:
	void DrawShadow();

	int id = 0;
	bool restartAnimation = true;
};