
#pragma once

#include <vector>
#include <memory>
#include "conditions/condition.h"
#include "conditions/condition_factory.h"

class State; //fd

struct WeightedStateTarget
{
    State* state = nullptr;
    float weight = 0.0f;
};

struct Transition {
    State* from;
    std::vector<WeightedStateTarget> targets;
    std::vector<std::unique_ptr<Condition>> conditions;

    State* PickTarget() const
    {
        float totalWeight = 0.0f;

        for (const WeightedStateTarget& target : targets) {
            totalWeight += target.weight;
        }

        if (totalWeight <= 0.0f) {
            printf("Transition has no positive target weights\n");
            return nullptr;
        }

        const float roll = static_cast<float>(rand()) /
            static_cast<float>(RAND_MAX) * totalWeight;

        float currentWeight = 0.0f;

        for (const WeightedStateTarget& target : targets) {
            currentWeight += target.weight;

            if (roll < currentWeight) {
                return target.state;
            }
        }

        // Protects against float rounding at the upper boundary.
        return targets.back().state;
    }
};

inline Transition BuildTransition(
    const Json& j,
    std::unordered_map<std::string, State*>& stateMap)
{
    Transition transition;
    transition.from = stateMap.at(j["from"].get<std::string>());

    const Json& destinations = j["to"];

    if (destinations.is_string()) {
        const std::string stateName = destinations.get<std::string>();

        transition.targets.push_back({
            stateMap.at(stateName),
            1.0f
            });
    }
    else if (destinations.is_array()) {
        for (const Json& targetJson : destinations) {
            const std::string stateName =
                targetJson.at("state").get<std::string>();

            const float weight =
                targetJson.at("weight").get<float>();

            if (weight <= 0.0f) {
                printf(
                    "Transition target '%s' must have a positive weight\n",
                    stateName.c_str()
                );
                continue;
            }

            transition.targets.push_back({
                stateMap.at(stateName),
                weight
                });
        }
    }
    else {
        printf("Transition 'to' must be a state name or target array\n");
    }

    for (const Json& conditionJson : j["conditions"]) {
        std::unique_ptr<Condition> condition =
            ConditionFactory::Instance().Create(conditionJson);

        if (condition) {
            transition.conditions.push_back(std::move(condition));
        }
    }

    return transition;
}