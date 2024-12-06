#pragma once

#include <string>

class Action
{
public:
    Action();
    Action(const std::string& name, const std::string& type);

    const std::string& GetName() const;
    const std::string& GetType() const;
    std::string ToString() const;

private:
    std::string name = "NONE";
    std::string type = "NONE";
};
