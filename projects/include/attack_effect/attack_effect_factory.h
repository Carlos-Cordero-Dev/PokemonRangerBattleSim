#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "json_loader.h"

class AttackEffect;

class AttackEffectFactory {
public:
    using Creator = std::function<std::unique_ptr<AttackEffect>(const Json&)>;

    static AttackEffectFactory& Instance() {
        static AttackEffectFactory instance;
        return instance;
    }

    void Register(const std::string& type, Creator creator) {
        creators[type] = creator;
    }

    std::unique_ptr<AttackEffect> Create(const Json& j) const {
        if (!j.contains("type") || !j["type"].is_string()) {
            printf("Attack effect is missing a string type\n");
            return nullptr;
        }

        const std::string type = j["type"];
        const auto it = creators.find(type);
        if (it == creators.end()) {
            printf("Unknown attack effect type: %s\n", type.c_str());
            return nullptr;
        }

        return it->second(j);
    }

private:
    std::unordered_map<std::string, Creator> creators;
};