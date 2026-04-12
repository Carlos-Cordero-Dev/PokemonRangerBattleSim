
#pragma once

#include <unordered_map>
#include <functional>
#include <memory>
#include <string>

#include "conditions/condition.h"
#include "json_loader.h"

class ConditionFactory {
public:
    using Creator = std::function<std::unique_ptr<Condition>(const Json&)>;

    static ConditionFactory& Instance() {
        static ConditionFactory instance;
        return instance;
    }

    void Register(const std::string& name, Creator func) {
        creators[name] = func;
    }

    std::unique_ptr<Condition> Create(const Json& j) {
        std::string type = j["type"];

        if (creators.count(type)) {
            return creators[type](j);
        }

        printf("Unknown Condition: %s\n", type.c_str());
        return nullptr;
    }

private:
    std::unordered_map<std::string, Creator> creators;
};