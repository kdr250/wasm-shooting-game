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

    static void Update(float deltaTime);

    inline static const std::string ENEMY_SPAWNER_TAG = "EnemySpawner";
    inline static const std::string EVENT_HANDLER_TAG = "EnemySpawnerEventHandler";

private:
    static std::shared_ptr<Entity>& GetSpawner();

    static std::shared_ptr<Entity>& GetEventHandler();

    static void ReadEnemyConfiguration(std::stringstream& stream,
                                       std::map<std::string, std::string>& config);

    static void ReadComponentConfiguration(std::stringstream& stream,
                                           std::map<std::string, std::string>& config);

    static void ReadEventConfiguration(std::stringstream& stream,
                                       std::map<std::string, std::string>& config);

    // Register Components
    static void RegisterComponents(std::shared_ptr<Entity>& enemy,
                                   std::map<std::string, std::string> config);

    static void RegisterSpriteComponent(std::shared_ptr<Entity>& enemy,
                                        std::map<std::string, std::string> config);

    static void RegisterMoveComponent(std::shared_ptr<Entity>& enemy,
                                      std::map<std::string, std::string> config);

    static void RegisterSplineMoveComponent(std::shared_ptr<Entity>& enemy,
                                            std::map<std::string, std::string> config);

    static void RegisterHealthComponent(std::shared_ptr<Entity>& enemy,
                                        std::map<std::string, std::string> config);

    // Register Events
    static void RegisterEvents(std::shared_ptr<Entity>& enemy,
                               std::map<std::string, std::string> config);

    static void RegisterSceneClearEvent(std::shared_ptr<Entity>& enemy,
                                        std::map<std::string, std::string> config);

    static void RegisterMoveEvent(const std::string& eventId,
                                  std::shared_ptr<Entity>& enemy,
                                  std::map<std::string, std::string> config,
                                  std::vector<std::function<Result(long, int)>>& events);

    static void RegisterSplineMoveEvent(const std::string& eventId,
                                        std::shared_ptr<Entity>& enemy,
                                        std::map<std::string, std::string> config,
                                        std::vector<std::function<Result(long, int)>>& events);

    static void RegisterExplosionBulletsEvent(
        const std::string& eventId,
        std::shared_ptr<Entity>& enemy,
        std::map<std::string, std::string> config,
        std::vector<std::function<Result(long, int)>>& events);

    static void RegisterRollBulletsEvent(const std::string& eventId,
                                         std::shared_ptr<Entity>& enemy,
                                         std::map<std::string, std::string> config,
                                         std::vector<std::function<Result(long, int)>>& events);

    static void RegisterSequentialBulletsEvent(
        const std::string& eventId,
        std::shared_ptr<Entity>& enemy,
        std::map<std::string, std::string> config,
        std::vector<std::function<Result(long, int)>>& events);

    static void RegisterWinderBulletsEvent(const std::string& eventId,
                                           std::shared_ptr<Entity>& enemy,
                                           std::map<std::string, std::string> config,
                                           std::vector<std::function<Result(long, int)>>& events);

    static std::string GenerateEventId(const std::string eventId);
    static std::string GenerateEventId(const int eventId);

    inline static const std::string ENEMY_SHOOT_SOUND_NAME = "shoot_enemy";
    inline static const std::string ENEMY_SHOOT_SOUND_PATH = "resources/audio/enemy.wav";
};
