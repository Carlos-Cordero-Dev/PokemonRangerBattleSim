#pragma once

#include <string>

class AttackEffect {
public:
    std::string name;
    virtual ~AttackEffect() = default;
};