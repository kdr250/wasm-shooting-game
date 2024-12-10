#pragma once

#include "Scene.h"

class Entity;

class ScenePlay : public Scene
{
public:
    ScenePlay(const int sceneId);

    void Update(float deltaTime) override;
    void DoAction(const Action& action) override;
    void Render() override;

    void MoveEntities(float deltaTime);
    void ProcessLifespan(float deltaTime);
    void ProcessCollision();

private:
    void OnEnd() override;

    void SetPause(bool pause);

    bool paused = false;

#ifdef __EMSCRIPTEN__
    inline static const std::string SPRITE_SHADER_VERT = "resources/shader/Sprite.vert";
    inline static const std::string SPRITE_SHADER_FRAG = "resources/shader/Sprite.frag";
    inline static const std::string BULLET_SHADER_VERT = "resources/shader/Bullet.vert";
    inline static const std::string BULLET_SHADER_FRAG = "resources/shader/Bullet.frag";
#else
    inline static const std::string SPRITE_SHADER_VERT = "resources/shader/SpriteV3.vert";
    inline static const std::string SPRITE_SHADER_FRAG = "resources/shader/SpriteV3.frag";
    inline static const std::string BULLET_SHADER_VERT = "resources/shader/BulletV3.vert";
    inline static const std::string BULLET_SHADER_FRAG = "resources/shader/BulletV3.frag";
#endif
    inline static const std::string SPRITE_SHADER_NAME = "sprite";
    inline static const std::string BULLET_SHADER_NAME = "bullet";
    inline static const std::string TITLE              = "title";

    inline static const std::string PLAYER_TEXTURE_NAME = "example";
    inline static const std::string PLAYER_TEXTURE      = "resources/texture/example.png";

    inline static const std::string ENEMY_TEXTURE_NAME = "example_enemy";
    inline static const std::string ENEMY_TEXTURE      = "resources/texture/example_enemy.png";

    inline static const std::string FONT_PATH = "resources/font/Roboto-Bold.ttf";
    inline static const std::string FONT_NAME = "Roboto-Bold";
};
