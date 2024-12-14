#pragma once

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

class Entity;

class EnemySpawner
{
public:
    static void Initialize(const int sceneId);

    static void Spawn(float deltaTime);

    inline static const std::string ENEMY_SPAWNER_TAG = "EnemySpawner";

private:
    static std::shared_ptr<Entity>& GetSpawner();

    static void ReadEnemyConfiguration(std::stringstream& stream,
                                       std::map<std::string, std::string>& config);

    static void ReadComponentConfiguration(std::stringstream& stream,
                                           std::map<std::string, std::string>& config);

    static void ReadEventConfiguration(std::stringstream& stream,
                                       std::map<std::string, std::string>& config);
};
