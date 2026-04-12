#pragma once

#include <vector>
#include <memory>
#include <string>

#include "world_object.h"
#include "actions/action.h"
#include "actions/action_factory.h"
#include "json_loader.h"

class State {
public:
    WorldObject* owner = nullptr;

    std::vector<std::unique_ptr<Action>> onEnterActions;
    std::vector<std::unique_ptr<Action>> onUpdateActions;
    std::vector<std::unique_ptr<Action>> onExitActions;

    virtual void OnEnter() {
        for (auto& a : onEnterActions)
            a->Execute(owner, 0.0f);
    }

    virtual void OnUpdate(float dt) {
        for (auto& a : onUpdateActions)
            a->Execute(owner, dt);
    }

    virtual void OnExit() {
        for (auto& a : onExitActions)
            a->Execute(owner, 0.0f);
    }

    virtual ~State() = default;
};

inline State* BuildState(const Json& j, WorldObject* owner) {
    State* s = new State();
    s->owner = owner;

    auto& af = ActionFactory::Instance();

    if (j.contains("onEnter"))
    {
        for (auto& a : j["onEnter"]) {
            s->onEnterActions.push_back(af.Create(a));
        }
    }
    if (j.contains("onUpdate"))
    {
        for (auto& a : j["onUpdate"]) {
            s->onUpdateActions.push_back(af.Create(a));
        }
    }
    if (j.contains("onExit"))
    {
        for (auto& a : j["onExit"]) {
            s->onExitActions.push_back(af.Create(a));
        }
    }

    return s;
}