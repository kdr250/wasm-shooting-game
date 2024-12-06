#pragma once

#include <string>

class Action
{
public:
    Action();
    Action(const std::string& name, const std::string& type);

    std::string name = "NONE";
    std::string type = "NONE";

    std::string ToString() const;
};
