
#pragma once
#include <unordered_map>
#include <string>
#include <raylib.h>

struct Blackboard {
    std::unordered_map<std::string, float> floats;
    std::unordered_map<std::string, Vector2> vectors;
    std::unordered_map<std::string, bool> bools;
};