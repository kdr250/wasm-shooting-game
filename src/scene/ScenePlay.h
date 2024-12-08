#pragma once

#include <memory>
#define GLM_FORCE_PURE
#include <glm/vec2.hpp>
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

private:
    void OnEnd() override;

    void ProcessPause();
    void SetPause(bool pause);

    void SpawnBullet(const glm::vec2& position,
                     const glm::vec2& velocity = glm::vec2 {0.0f, -600.0f},
                     const float size          = 200.0f);

    std::shared_ptr<Entity> player;

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

    inline static const std::string FONT_PATH = "resources/font/Roboto-Bold.ttf";
    inline static const std::string FONT_NAME = "Roboto-Bold";
};
