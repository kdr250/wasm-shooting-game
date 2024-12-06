#include "Action.h"

Action::Action() {}

Action::Action(const std::string& name, const std::string& type) : name(name), type(type) {}

std::string Action::ToString() const
{
    return "{ name: " + name + ", type: " + type + " }";
}
