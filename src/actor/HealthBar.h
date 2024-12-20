#pragma once

#include <memory>
#include <string>
#define GLM_FORCE_PURE
#include <glm/vec2.hpp>

class Entity;

class HealthBar
{
public:
    static void Initialize(const int maxHP, const glm::vec2& position);
    static void Draw();

    static void Unload();

private:
    static std::shared_ptr<Entity>& GetHealthBar();

    static int health;
    static int maxHealth;

#ifdef __EMSCRIPTEN__
    inline static const std::string SPRITE_SHADER_VERT = "resources/shader/Sprite.vert";
    inline static const std::string SPRITE_SHADER_FRAG = "resources/shader/Sprite.frag";
#else
    inline static const std::string SPRITE_SHADER_VERT = "resources/shader/SpriteV3.vert";
    inline static const std::string SPRITE_SHADER_FRAG = "resources/shader/SpriteV3.frag";
#endif
    inline static const std::string SPRITE_SHADER_NAME = "sprite";

    inline static const std::string HEALTH_BAR_TEXTURE_PATH = "resources/texture/example_enemy.png";
    inline static const std::string HEALTH_BAR_TEXTURE_NAME = "HealthBar";

    inline static const std::string HEALTH_BAR_TAG = "HealthBar";
};
