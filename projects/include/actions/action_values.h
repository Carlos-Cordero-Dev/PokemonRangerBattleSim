
#pragma once

#include "world_object.h"

class FloatValue {
public:
    virtual float Get() = 0;
    virtual ~FloatValue() = default;
};

class ConstFloat : public FloatValue {
public:
    float value;

    float Get() override {
        return value;
    }
};

class RandomRangeValue : public FloatValue {
public:
    float min, max;

    float Get() override {
        return min + (float)rand() / RAND_MAX * (max - min);
    }
};