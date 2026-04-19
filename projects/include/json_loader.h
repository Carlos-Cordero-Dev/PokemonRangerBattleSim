
#pragma once

#include <fstream>
#include "actions/action_values.h"

#include <external/nlohmann/json.hpp>

using Json = nlohmann::json;

inline Json LoadJson(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open()) {
        printf("Failed to open file: %s\n", path.c_str());
        return {};
    }

    Json j;
    file >> j;

    return j;
}

inline std::unique_ptr<FloatValue> ParseFloatValue(const Json& j)
{
    // case 1: plain number
    if (j.is_number()) {
        auto v = std::make_unique<ConstFloat>();
        v->value = j;
        return v;
    }

    // case 2: object
    std::string type = j["type"];

    if (type == "random_range") {
        auto v = std::make_unique<RandomRangeValue>();
        v->min = j["min"];
        v->max = j["max"];
        return v;
    }

    printf("Unknown value type\n");
    return nullptr;
}