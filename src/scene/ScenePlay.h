#pragma once

#include "Scene.h"

class Entity;
class Texture;

class ScenePlay : public Scene
{
public:
    ScenePlay(const int sceneId);

    virtual void Update(float deltaTime) override;
    virtual void DoAction(const Action& action) override;
    void Render() override;

    virtual void OnEnd() override;

    void MoveEntities(float deltaTime);

private:
    std::shared_ptr<Entity> player;

#ifdef __EMSCRIPTEN__
    inline static const std::string SPRITE_SHADER_VERT = "resources/shader/Sprite.vert";
    inline static const std::string SPRITE_SHADER_FRAG = "resources/shader/Sprite.frag";
#else
    inline static const std::string SPRITE_SHADER_VERT = "resources/shader/SpriteV3.vert";
    inline static const std::string SPRITE_SHADER_FRAG = "resources/shader/SpriteV3.frag";
#endif

    inline static const std::string PLAYER_TEXTURE = "resources/texture/example.png";
    inline static const std::string FONT_PATH      = "resources/font/Roboto-Bold.ttf";
    inline static const std::string FONT_NAME      = "Roboto-Bold";
};
