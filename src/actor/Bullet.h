#pragma once

#include <memory>
#include <string>
#include <vector>
#define GLM_FORCE_PURE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class Entity;

class Bullet
{
public:
    static constexpr glm::vec3 GREEN = {0.0f, 1.0f, 0.0f};
    static constexpr glm::vec3 RED   = {1.0f, 0.0f, 0.0f};

    static void SpawnDirectionalBullet(const glm::vec2& position,
                                       const glm::vec2& velocity,
                                       const glm::vec3& color,
                                       const std::string& ownerTag,
                                       const float size);

    static void SpawnExplosionBullets(const glm::vec2& position,
                                      const glm::vec3& color,
                                      const int bulletsNum,
                                      const std::string& ownerTag,
                                      const float speed,
                                      const float size);

    static void Move(float deltaTime);
    static void Draw();

    static const std::vector<std::shared_ptr<Entity>>& GetBullets();

private:
    static constexpr float PI = 3.1415926535f;

#ifdef __EMSCRIPTEN__
    inline static const std::string BULLET_SHADER_VERT = "resources/shader/Bullet.vert";
    inline static const std::string BULLET_SHADER_FRAG = "resources/shader/Bullet.frag";
#else
    inline static const std::string BULLET_SHADER_VERT = "resources/shader/BulletV3.vert";
    inline static const std::string BULLET_SHADER_FRAG = "resources/shader/BulletV3.frag";
#endif
    inline static const std::string SPRITE_SHADER_NAME = "sprite";
    inline static const std::string BULLET_SHADER_NAME = "bullet";

    inline static const std::string BULLET_TAG = "bullet";
};
