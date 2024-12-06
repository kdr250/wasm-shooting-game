#include "Action.h"

Action::Action() {}

Action::Action(const std::string& name, const std::string& type) : name(name), type(type) {}

const std::string& Action::GetName() const
{
    return name;
}

const std::string& Action::GetType() const
{
    return type;
}

std::string Action::ToString() const
{
    return "{ name: " + name + ", type: " + type + " }";
}
