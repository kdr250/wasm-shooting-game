#pragma once

#include <memory>
#include <vector>

class Entity;

class EnemySpawner
{
public:
    static std::vector<std::shared_ptr<Entity>> Spawn(const int sceneId);
};
