#pragma once

#include <algorithm>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "attack_effect.h"
#include "attack_effect_factory.h"
#include "animation_database.h"

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

            if (effectJson.contains("visualAnimation")) {
                if (!effectJson["visualAnimation"].is_string()) {
                    printf("Attack effect '%s' has an invalid visualAnimation\n", name.c_str());
                    return false;
                }

                const std::string animationName = effectJson["visualAnimation"];
                if (!AnimationDatabase::Instance().LoadAnimDataByName(animationName)) {
                    printf("Attack effect '%s' could not load animation '%s'\n",
                        name.c_str(), animationName.c_str());
                    return false;
                }
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

    bool LoadFromFolder(const std::string& path) {
        if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
            printf("Attack effects folder does not exist: %s\n", path.c_str());
            return false;
        }

        std::vector<std::filesystem::path> jsonPaths;
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json")
                jsonPaths.push_back(entry.path());
        }

        std::sort(jsonPaths.begin(), jsonPaths.end());

        bool loadedAll = true;
        for (const std::filesystem::path& jsonPath : jsonPaths) {
            if (!LoadFromFile(jsonPath.string())) {
                printf("Failed to load attack effects from: %s\n", jsonPath.string().c_str());
                loadedAll = false;
            }
        }

        return loadedAll;
    }

    const AttackEffect* Get(const std::string& name) const {
        auto it = effects.find(name);
        return it == effects.end() ? nullptr : it->second.get();
    }

private:
    std::unordered_map<std::string, std::unique_ptr<AttackEffect>> effects;
};