#include "Entity.h"

Entity::Entity(const size_t& id, const std::string& tag) : id(id), tag(tag) {}

void Entity::Destroy()
{
    active = false;
}

size_t Entity::GetId() const
{
    return id;
}

bool Entity::IsActive() const
{
    return active;
}

const std::string& Entity::GetTag() const
{
    return tag;
}
