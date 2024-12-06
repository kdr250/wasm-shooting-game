#pragma once

#include "Scene.h"

class SceneMenu : public Scene
{
public:
    SceneMenu();

    void Update(float deltaTime) override;
    void DoAction(const Action& action) override;
    void Render() override;

private:
    void OnEnd() override;

    bool pressEnter = false;

#ifdef __EMSCRIPTEN__
    inline static const std::string SPRITE_SHADER_VERT = "resources/shader/Sprite.vert";
    inline static const std::string SPRITE_SHADER_FRAG = "resources/shader/Sprite.frag";
#else
    inline static const std::string SPRITE_SHADER_VERT = "resources/shader/SpriteV3.vert";
    inline static const std::string SPRITE_SHADER_FRAG = "resources/shader/SpriteV3.frag";
#endif

    inline static const std::string SHADER_NAME = "sprite";
    inline static const std::string TITLE       = "title";

    inline static const std::string FONT_PATH = "resources/font/Roboto-Bold.ttf";
    inline static const std::string FONT_NAME = "Roboto-Bold";
};
