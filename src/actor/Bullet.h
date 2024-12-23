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
    static void Move(float deltaTime);
    static void Draw();

    static const std::vector<std::shared_ptr<Entity>>& GetBullets(const std::string& ownerTag);

    static std::string GenerateTagName(const std::string& ownerTag);

private:
    inline static const std::string BULLET_TAG_PREFIX = "bullet";
};
