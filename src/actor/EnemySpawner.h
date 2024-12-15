#pragma once

#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include "../ecs/Components.h"

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

    static void RegisterEvents(std::shared_ptr<Entity>& enemy,
                               std::map<std::string, std::string> config);

    static void RegisterMoveEvent(const int eventId,
                                  std::shared_ptr<Entity>& enemy,
                                  std::map<std::string, std::string> config,
                                  std::vector<std::function<Result(long, int)>>& events);

    static void RegisterSplineMoveEvent(const int eventId,
                                        std::shared_ptr<Entity>& enemy,
                                        std::map<std::string, std::string> config,
                                        std::vector<std::function<Result(long, int)>>& events);

    static void RegisterExplosionBulletsEvent(
        const int eventId,
        std::shared_ptr<Entity>& enemy,
        std::map<std::string, std::string> config,
        std::vector<std::function<Result(long, int)>>& events);

    static void RegisterRollBulletsEvent(const int eventId,
                                         std::shared_ptr<Entity>& enemy,
                                         std::map<std::string, std::string> config,
                                         std::vector<std::function<Result(long, int)>>& events);

    static void RegisterSequentialBulletsEvent(
        const int eventId,
        std::shared_ptr<Entity>& enemy,
        std::map<std::string, std::string> config,
        std::vector<std::function<Result(long, int)>>& events);

    static void RegisterWinderBulletsEvent(const int eventId,
                                           std::shared_ptr<Entity>& enemy,
                                           std::map<std::string, std::string> config,
                                           std::vector<std::function<Result(long, int)>>& events);
};
