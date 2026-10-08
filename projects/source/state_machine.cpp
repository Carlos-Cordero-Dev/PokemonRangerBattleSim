
#include "state_machine.h"



StateMachine::Attributes StateMachine::ParseAttributes(const Json& document)
{
    StateMachine::Attributes attributes;
    if (!document.contains("attributes")) return attributes;

    const Json& entries = document.at("attributes");
    if (!entries.is_array() || entries.size() != 1 || !entries.front().is_object())
        throw std::invalid_argument("State machine attributes must be an array containing one object");

    const Json& values = entries.front();
    if (values.contains("health")) {
        const float health = values.at("health").get<float>();
        if (!std::isfinite(health) || health <= 0.0f)
            throw std::invalid_argument("State machine health must be a positive finite number");
        attributes.health = health;
    }

    if (values.contains("types"))
        attributes.types = values.at("types").get<std::vector<std::string>>();

    if (values.contains("damage")) {
        const Json& damageEntries = values.at("damage");
        if (!damageEntries.is_array())
            throw std::invalid_argument("State machine damage must be an array");

        for (const Json& entry : damageEntries) {
            const std::string stateName = entry.at("state_name").get<std::string>();
            const Json& damage = entry.at("damage");
            if (!damage.is_number_integer() || damage < 0 || damage > std::numeric_limits<int>::max())
                throw std::invalid_argument("Damage for state '" + stateName + "' must be a non-negative integer within int range");

            bool stateExists = false;
            for (const Json& state : document.at("states")) {
                if (state.at("name").get<std::string>() == stateName) {
                    stateExists = true;
                    break;
                }
            }
            if (!stateExists)
                throw std::invalid_argument("Damage references unknown state '" + stateName + "'");

            if (!attributes.damageByState.emplace(stateName, damage.get<int>()).second)
                throw std::invalid_argument("Duplicate damage entry for state '" + stateName + "'");
        }
    }

    return attributes;
}