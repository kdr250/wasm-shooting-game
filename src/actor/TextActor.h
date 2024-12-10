#pragma once

#include <memory>
#include <string>
#include <vector>
#define GLM_FORCE_PURE
#include <glm/vec2.hpp>

class Entity;

class TextActor
{
public:
    static void Spawn(const std::string& text, const glm::vec2& position);
    static void Draw();

    static void Unload();

private:
    static const std::vector<std::shared_ptr<Entity>>& GetTextActors();

#ifdef __EMSCRIPTEN__
    inline static const std::string SPRITE_SHADER_VERT = "resources/shader/Sprite.vert";
    inline static const std::string SPRITE_SHADER_FRAG = "resources/shader/Sprite.frag";
#else
    inline static const std::string SPRITE_SHADER_VERT = "resources/shader/SpriteV3.vert";
    inline static const std::string SPRITE_SHADER_FRAG = "resources/shader/SpriteV3.frag";
#endif
    inline static const std::string SPRITE_SHADER_NAME = "sprite";

    inline static const std::string FONT_PATH = "resources/font/Roboto-Bold.ttf";
    inline static const std::string FONT_NAME = "Roboto-Bold";

    inline static const std::string TEXT_TAG = "text";
};
