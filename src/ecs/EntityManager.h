#pragma once

#include <map>
#include <memory>
#include <vector>
#include "Entity.h"

using EntityVector = std::vector<std::shared_ptr<Entity>>;

class EntityManager
{
public:
    EntityManager();

    void Update();
    std::shared_ptr<Entity> AddEntity(const std::string& tag);
    EntityVector& GetEntities();
    EntityVector& GetEntities(const std::string& tag);

private:
    void RemoveDeadEntities(EntityVector& vec);

    EntityVector entities;                          // All entities
    EntityVector entitiesToAdd;                     // Entities to add next update
    std::map<std::string, EntityVector> entityMap;  // Map from entity tag to vectors
    size_t totalEntities = 0;
};
