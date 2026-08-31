
#pragma once

#include <vector>
#include <memory>

#include "blackboard.h"
#include "states/state.h"
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

private:
    bool running = false;

public:
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

    std::unordered_map<std::string, State*> stateMap;

    // states
    for (auto& s : j["states"]) {
        std::string name = s["name"];
        State* state = BuildState(s,owner);
        stateMap[name] = state;
        sm->ownedStates.push_back(state);
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