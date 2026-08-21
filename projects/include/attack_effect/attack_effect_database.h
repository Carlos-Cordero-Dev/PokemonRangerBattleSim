#pragma once

#include <string>
#include <unordered_map>

#include "attack_effect.h"
#include "attack_effect_factory.h"

class AttackEffectDatabase {
public:
    static AttackEffectDatabase& Instance() {
        static AttackEffectDatabase instance;
        return instance;
    }

    bool LoadFromJson(const Json& j) {
        if (!j.contains("effects") || !j["effects"].is_array()) {
            printf("Attack effect JSON must contain an 'effects' array\n");
            return false;
        }

        for (const auto& effectJson : j["effects"]) {
            if (!effectJson.contains("name") || !effectJson["name"].is_string()) {
                printf("Attack effect is missing a string name\n");
                return false;
            }

            const std::string name = effectJson["name"];

            if (effects.count(name) != 0) {
                printf("Duplicate attack effect: %s\n", name.c_str());
                return false;
            }

            std::unique_ptr<AttackEffect> effect = AttackEffectFactory::Instance().Create(effectJson);
            if (!effect) {
                return false;
            }

            effect->name = name;
            effects.emplace(name, std::move(effect));
        }

        return true;
    }

    bool LoadFromFile(const std::string& path) {
        return LoadFromJson(LoadJson(path));
    }

    const AttackEffect* Get(const std::string& name) const {
        auto it = effects.find(name);
        return it == effects.end() ? nullptr : it->second.get();
    }

private:
    std::unordered_map<std::string, std::unique_ptr<AttackEffect>> effects;
};