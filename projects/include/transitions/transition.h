
#pragma once

#include <vector>
#include <memory>
#include "conditions/condition.h"
#include "conditions/condition_factory.h"

class State; //fd

struct Transition {
    State* from;
    State* to;
    std::vector<std::unique_ptr<Condition>> conditions;
};

inline Transition BuildTransition(
    const Json& j,
    std::unordered_map<std::string, State*>& stateMap)
{
    Transition t;

    t.from = stateMap[j["from"]];
    t.to = stateMap[j["to"]];

    auto& cf = ConditionFactory::Instance();

    for (auto& c : j["conditions"]) {
        t.conditions.push_back(cf.Create(c));
    }

    return t;
}