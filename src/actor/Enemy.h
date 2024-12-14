#pragma once

#include <memory>
#include <string>
#include <vector>
#define GLM_FORCE_PURE
#include <glm/vec2.hpp>

class Entity;

class Enemy
{
public:
    static std::vector<std::shared_ptr<Entity>> Spawn(const int sceneid);
    static void Move(float deltaTime);
    static void Collide();
    static void Draw();

    static void Unload();

    static const std::vector<std::shared_ptr<Entity>>& GetEnemies();

    inline static const std::string ENEMY_TAG = "enemy";

private:
#ifdef __EMSCRIPTEN__
    inline static const std::string SPRITE_SHADER_VERT = "resources/shader/Sprite.vert";
    inline static const std::string SPRITE_SHADER_FRAG = "resources/shader/Sprite.frag";
#else
    inline static const std::string SPRITE_SHADER_VERT = "resources/shader/SpriteV3.vert";
    inline static const std::string SPRITE_SHADER_FRAG = "resources/shader/SpriteV3.frag";
#endif
    inline static const std::string SPRITE_SHADER_NAME = "sprite";
    inline static const std::string TITLE              = "title";

    inline static const std::string ENEMY_TEXTURE_NAME = "example_enemy";
    inline static const std::string ENEMY_TEXTURE      = "resources/texture/example_enemy.png";
};
