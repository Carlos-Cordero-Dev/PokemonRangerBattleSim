
#pragma once

#include <unordered_map>
#include <functional>
#include <memory>
#include <string>

//#include "action.h"
#include "json_loader.h"

class Action; //fd

class ActionFactory {
public:
    using Creator = std::function<std::unique_ptr<Action>(const Json&)>;

    static ActionFactory& Instance() {
        static ActionFactory instance;
        return instance;
    }

    void Register(const std::string& name, Creator func) {
        creators[name] = func;
    }

    std::unique_ptr<Action> Create(const Json& j) {
        
        std::string type = j["type"];

        if (creators.count(type)) {
            return creators[type](j);
        }

        printf("Unknown Action: %s\n", type.c_str());
        return nullptr;
    }

private:
    std::unordered_map<std::string, Creator> creators;
};