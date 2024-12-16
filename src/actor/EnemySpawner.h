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

    static void ReadEnemyConfiguration(const int enemyCount,
                                       std::stringstream& stream,
                                       std::map<std::string, std::string>& config);

    static void ReadComponentConfiguration(const int enemyCount,
                                           std::stringstream& stream,
                                           std::map<std::string, std::string>& config);

    static void ReadEventConfiguration(const int enemyCount,
                                       std::stringstream& stream,
                                       std::map<std::string, std::string>& config);

    // Register Components
    static void RegisterComponents(const int enemyCount,
                                   std::shared_ptr<Entity>& enemy,
                                   std::map<std::string, std::string> config);

    static void RegisterSpriteComponent(const int enemyCount,
                                        std::shared_ptr<Entity>& enemy,
                                        std::map<std::string, std::string> config);

    static void RegisterMoveComponent(const int enemyCount,
                                      std::shared_ptr<Entity>& enemy,
                                      std::map<std::string, std::string> config);

    static void RegisterSplineMoveComponent(const int enemyCount,
                                            std::shared_ptr<Entity>& enemy,
                                            std::map<std::string, std::string> config);

    // Register Events
    static void RegisterEvents(const int enemyCount,
                               std::shared_ptr<Entity>& enemy,
                               std::map<std::string, std::string> config);

    static void RegisterMoveEvent(const int enemyCount,
                                  const int eventId,
                                  std::shared_ptr<Entity>& enemy,
                                  std::map<std::string, std::string> config,
                                  std::vector<std::function<Result(long, int)>>& events);

    static void RegisterSplineMoveEvent(const int enemyCount,
                                        const int eventId,
                                        std::shared_ptr<Entity>& enemy,
                                        std::map<std::string, std::string> config,
                                        std::vector<std::function<Result(long, int)>>& events);

    static void RegisterExplosionBulletsEvent(
        const int enemyCount,
        const int eventId,
        std::shared_ptr<Entity>& enemy,
        std::map<std::string, std::string> config,
        std::vector<std::function<Result(long, int)>>& events);

    static void RegisterRollBulletsEvent(const int enemyCount,
                                         const int eventId,
                                         std::shared_ptr<Entity>& enemy,
                                         std::map<std::string, std::string> config,
                                         std::vector<std::function<Result(long, int)>>& events);

    static void RegisterSequentialBulletsEvent(
        const int enemyCount,
        const int eventId,
        std::shared_ptr<Entity>& enemy,
        std::map<std::string, std::string> config,
        std::vector<std::function<Result(long, int)>>& events);

    static void RegisterWinderBulletsEvent(const int enemyCount,
                                           const int eventId,
                                           std::shared_ptr<Entity>& enemy,
                                           std::map<std::string, std::string> config,
                                           std::vector<std::function<Result(long, int)>>& events);

    static std::string GenerateEventId(const int enemyCount, const std::string eventId);
    static std::string GenerateEventId(const int enemyCount, const int eventId);
};
