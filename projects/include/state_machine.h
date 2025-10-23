
#pragma once

#include <vector>

class WorldObject; //fd

class State {
public:
    WorldObject* owner = nullptr;

	virtual void OnEnter() {}
    virtual State* OnUpdate(float dt) { return nullptr; }
	virtual void OnExit() {}
    virtual ~State() = default;
};

class StateMachine {

public:
    State* currentState = nullptr;

    WorldObject* owner = nullptr;
    std::vector<State*> ownedStates;

private:
    bool running = false;

public:
    
    void Update(float dt) {
        if (!running)
        {
            currentState->OnEnter();
            running = true;
        }
        State* nextState = currentState->OnUpdate(dt);
        if (nextState)
        {
            currentState->OnExit();
            nextState->OnEnter();
            currentState = nextState;
        }
    }
};

