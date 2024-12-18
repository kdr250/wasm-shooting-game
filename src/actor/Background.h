#pragma once

#include <memory>
#include <string>
#include <vector>

class Entity;

class Background
{
public:
    static void Spawn();

    static void Draw();

private:
    static const std::vector<std::shared_ptr<Entity>>& GetBackgrounds();

#ifdef __EMSCRIPTEN__
    inline static const std::string SPRITE_SHADER_VERT = "resources/shader/Sprite.vert";
    inline static const std::string SPRITE_SHADER_FRAG = "resources/shader/Sprite.frag";
#else
    inline static const std::string SPRITE_SHADER_VERT = "resources/shader/SpriteV3.vert";
    inline static const std::string SPRITE_SHADER_FRAG = "resources/shader/SpriteV3.frag";
#endif
    inline static const std::string SPRITE_SHADER_NAME = "sprite";

    inline static const std::string BACKGROUND_PATH_ONE = "resources/texture/background1.png";
    inline static const std::string BACKGROUND_NAME_ONE = "background1";
    inline static const std::string BACKGROUND_PATH_TWO = "resources/texture/background2.png";
    inline static const std::string BACKGROUND_NAME_TWO = "background2";

    inline static const std::string BACKGROUND_TAG = "background";
};
