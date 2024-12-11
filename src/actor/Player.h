#pragma once

#include <memory>
#include <string>
#define GLM_FORCE_PURE
#include <glm/vec2.hpp>

class Entity;
class Action;

class Player
{
public:
    static std::shared_ptr<Entity> Spawn(const glm::vec2& position);
    static void ProcessAction(const Action& action);
    static bool NeedsPause();
    static void Move(float deltaTime);
    static bool IsCollide();
    static void Draw();

    static void Unload();

    static std::shared_ptr<Entity>& GetPlayer();

private:
#ifdef __EMSCRIPTEN__
    inline static const std::string SPRITE_SHADER_VERT = "resources/shader/Sprite.vert";
    inline static const std::string SPRITE_SHADER_FRAG = "resources/shader/Sprite.frag";
#else
    inline static const std::string SPRITE_SHADER_VERT = "resources/shader/SpriteV3.vert";
    inline static const std::string SPRITE_SHADER_FRAG = "resources/shader/SpriteV3.frag";
#endif
    inline static const std::string SPRITE_SHADER_NAME = "sprite";

    inline static const std::string PLAYER_TEXTURE_NAME = "example";
    inline static const std::string PLAYER_TEXTURE      = "resources/texture/example.png";

    inline static const std::string PLAYER_TAG = "player";
};
