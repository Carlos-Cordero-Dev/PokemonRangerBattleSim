#pragma once

#include <algorithm>
#include <string>

#include "action.h"
#include "action_values.h"
#include "pokemon.h"
#include "visual_effect.h"
#include "world.h"

class SpawnVisualEffectAction : public Action {
public:
	std::string animationName;
	std::unique_ptr<FloatValue> offsetX;
	std::unique_ptr<FloatValue> offsetY;
	std::unique_ptr<FloatValue> scale;
	std::unique_ptr<FloatValue> rotationDeg;
	bool followOwner = true;
	bool drawBehindOwner = true;
	VisualEffect* activeEffect = nullptr;

	void Execute(WorldObject* owner, float dt) override {
		RemoveActiveEffect();

		Pokemon* pokemon = static_cast<Pokemon*>(owner);
		const AnimationVariant* animation = pokemon->ResolveAnimation(animationName);
		if (!animation || !animation->data) {
			printf("Unknown visual animation '%s'\n", animationName.c_str());
			return;
		}

		activeEffect = new VisualEffect(
			animation->data,
			owner,
			animationName,
			{
				offsetX->Get() * pokemon->scale,
				offsetY->Get() * pokemon->scale
			},
			scale->Get() * pokemon->scale,
			rotationDeg->Get() + pokemon->rotationDeg,
			animation->flipX,
			followOwner,
			drawBehindOwner);
		GameManager::GetInstance().activeVisualEffects.push_back(activeEffect);
	}

	void OnStateExit(WorldObject* owner) override {
		RemoveActiveEffect();
	}

private:
	void RemoveActiveEffect() {
		if (!activeEffect)
			return;

		auto& effects = GameManager::GetInstance().activeVisualEffects;
		effects.erase(std::remove(effects.begin(), effects.end(), activeEffect), effects.end());
		delete activeEffect;
		activeEffect = nullptr;
	}
};