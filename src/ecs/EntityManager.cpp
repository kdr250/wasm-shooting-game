#include "EntityManager.h"
#include <algorithm>

EntityManager::EntityManager() {}

void EntityManager::Update()
{
    for (auto entity : entitiesToAdd)
    {
        entities.push_back(entity);
        entityMap[entity->GetTag()].push_back(entity);
    }

    entitiesToAdd.clear();

    RemoveDeadEntities(entities);

    for (auto& [tag, vec] : entityMap)
    {
        RemoveDeadEntities(vec);
    }
}

std::shared_ptr<Entity> EntityManager::AddEntity(const std::string& tag)
{
    auto entity = std::make_shared<Entity>(totalEntities, tag);
    entitiesToAdd.push_back(entity);
    totalEntities++;
    return entity;
}

EntityVector& EntityManager::GetEntities()
{
    return entities;
}

EntityVector& EntityManager::GetEntities(const std::string& tag)
{
    return entityMap[tag];
}

bool EntityManager::HasEntities(const std::string& tag)
{
    return entityMap.contains(tag) && !entityMap[tag].empty();
}

bool EntityManager::HasEntity(const int id)
{
    auto iter = std::find_if(entities.begin(),
                             entities.end(),
                             [id](auto& entity)
                             {
                                 return entity->GetId() == id;
                             });
    return iter != entities.end();
}

void EntityManager::RemoveDeadEntities(EntityVector& vec)
{
    auto result = std::remove_if(vec.begin(),
                                 vec.end(),
                                 [](std::shared_ptr<Entity>& entity)
                                 {
                                     return !entity->IsActive();
                                 });
    vec.erase(result, vec.end());
}
