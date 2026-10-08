
#pragma once

#include <vector>
#include <memory>
#include <cmath>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include "blackboard.h"
#include "states/state.h"
#include "states/damaging_state.h"
#include "transitions/transition.h"
#include "actions/action.h"
#include "json_loader.h"

class WorldObject; //fd

class StateMachine {
public:
    State* currentState = nullptr;
    WorldObject* owner = nullptr;

    std::vector<State*> ownedStates;
    std::vector<Transition> transitions;

    Blackboard blackboard;

    struct Attributes {
        std::optional<float> health;
        std::optional<std::vector<std::string>> types;
        std::unordered_map<std::string, int> damageByState;
    };

    Attributes attributes;

private:

    bool running = false;

public:
    int GetDamageForState(const std::string& stateName) const {
        const auto entry = attributes.damageByState.find(stateName);
        return entry != attributes.damageByState.end() ? entry->second : 0;
    }

    // TODO: maybe this can be a bottleneck cause why tf is this on update bro, just cache the damage on state change
    inline int GetCurrentDamage() const {
		DamagingState* currentState = dynamic_cast<DamagingState*>(this->currentState);
        return currentState ? currentState->damage : 0;
    }

    Attributes ParseAttributes(const Json& document);

    void Update(float dt) {
        if (!running) {
            currentState->OnEnter();
            running = true;
        }

        currentState->OnUpdate(dt);

        // check transitions
        for (auto& t : transitions) {
            if (t.from != currentState)
                continue;

            bool valid = true;

            for (auto& cond : t.conditions) {
                if (!cond->Evaluate(owner)) {
                    valid = false;
                    break;
                }
            }

            if (valid) {
                State* nextState = t.PickTarget();

                if (nextState) {
                    currentState->OnExit();
                    currentState = nextState;
                    currentState->OnEnter();
                }

                break;
            }
        }
    }
};

inline StateMachine* BuildStateMachine(const Json& j, WorldObject* owner)
{
    auto* sm = new StateMachine();
    sm->owner = owner;
    sm->attributes = sm->ParseAttributes(j);

    std::unordered_map<std::string, State*> stateMap;

    // states
    for (auto& s : j["states"]) {
        std::string name = s["name"];
        State* state = BuildState(s,owner);
        stateMap[name] = state;

        //optional damage for damaging states
		DamagingState* damagingState = dynamic_cast<DamagingState*>(state);
        if (damagingState)
        {
            damagingState->damage = sm->GetDamageForState(name);

            sm->ownedStates.push_back(damagingState);
        }
        else sm->ownedStates.push_back(state);
    }

    // transitions
    for (auto& t : j["transitions"]) {
        sm->transitions.push_back(
            BuildTransition(t, stateMap)
        );
    }

    // initial state
    sm->currentState = stateMap[j["initialState"]];

    return sm;
}