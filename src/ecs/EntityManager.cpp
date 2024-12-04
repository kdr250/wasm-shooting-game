#include "EntityManager.h"

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
