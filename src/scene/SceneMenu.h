#pragma once

#include "Scene.h"

class SceneMenu : public Scene
{
public:
    SceneMenu();

    void Update(float deltaTime) override;
    void DoAction(const Action& action) override;
    void Render() override;
    void OnClear() override {};
    void OnOver() override {};

private:
    void OnEnd() override;

    inline static const std::string SPRITE_SHADER_VERT = "resources/shader/Sprite.vert";
    inline static const std::string SPRITE_SHADER_FRAG = "resources/shader/Sprite.frag";
    inline static const std::string SHADER_NAME        = "sprite";

    inline static const std::string TITLE = "title";

    inline static const std::string FONT_PATH = "resources/font/Roboto-Bold.ttf";
    inline static const std::string FONT_NAME = "Roboto-Bold";
};
