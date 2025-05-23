#pragma once

#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include "../ecs/Components.h"

class Entity;

class BulletSpawner
{
public:
    static constexpr float PI        = 3.1415926535f;
    static constexpr glm::vec3 GREEN = {0.0f, 1.0f, 0.0f};
    static constexpr glm::vec3 RED   = {1.0f, 0.0f, 0.0f};

    static void Initialize();
    static void Spawn(float deltaTime);

    static void RegisterSpawnDirectionalBullet(const glm::vec2& position,
                                               const glm::vec2& velocity,
                                               const glm::vec3& color,
                                               const std::string& ownerTag,
                                               const float size,
                                               const std::string& soundName);

    static void RegisterSpawnExplosionBullets(const glm::vec2& position,
                                              const glm::vec3& color,
                                              const int bulletsNum,
                                              const std::string& ownerTag,
                                              const float speed,
                                              const float size,
                                              const std::string& soundName);

    static void SpawnRollBullets(const glm::vec2& position,
                                 const glm::vec3& color,
                                 const int bulletsNum,
                                 const std::string& ownerTag,
                                 const float speed,
                                 const float size,
                                 const std::string& soundName);

    static void SpawnSequentialBullets(const glm::vec2& position,
                                       const glm::vec2& target,
                                       const int bulletsNum,
                                       const glm::vec3& color,
                                       const std::string& ownerTag,
                                       const float speed,
                                       const float size,
                                       const std::string& soundName);

    static void SpawnWinderBullets(const glm::vec2& position,
                                   const glm::vec2& target,
                                   const int loopNum,
                                   const glm::vec3& color,
                                   const std::string& ownerTag,
                                   const float speed,
                                   const float size,
                                   const std::string& soundName);

private:
    static void SpawnDirectionalBullet(const glm::vec2& position,
                                       const glm::vec2& velocity,
                                       const glm::vec3& color,
                                       const std::string& ownerTag,
                                       const float size,
                                       const std::string& soundName);

    static void SpawnExplosionBullets(const glm::vec2& position,
                                      const glm::vec3& color,
                                      const int bulletsNum,
                                      const std::string& ownerTag,
                                      const float speed,
                                      const float size,
                                      const std::string& soundName);

    static std::shared_ptr<Entity> GetSpawner(const std::string& ownerTag);

    static std::string GenerateSpawnerTagName(const std::string& ownerTag);

    inline static const std::string BULLET_SHADER_VERT = "resources/shader/Bullet.vert";
    inline static const std::string BULLET_SHADER_FRAG = "resources/shader/Bullet.frag";
    inline static const std::string SPRITE_SHADER_NAME = "sprite";
    inline static const std::string BULLET_SHADER_NAME = "bullet";

    inline static const std::string BULLET_TAG_PREFIX = "bullet";
    inline static const std::string SPAWNER_TAG       = "bullet_spawner";
};
