#include "ScenePlay.h"
#include <SDL2/SDL_scancode.h>
#include <algorithm>
#include <cmath>
#include "../Game.h"
#include "../actor/Enemy.h"
#include "../actor/Player.h"
#include "Action.h"
#include "SceneMenu.h"

const float PI        = 3.1415926535f;
const glm::vec3 GREEN = {0.0f, 1.0f, 0.0f};
const glm::vec3 RED   = {1.0f, 0.0f, 0.0f};

ScenePlay::ScenePlay(const int sceneId) : Scene(sceneId)
{
    auto& game          = Game::GetGame();
    auto& assetManager  = game.GetAssetManager();
    auto& entityManager = game.GetEntityManger();

    if (!assetManager.LoadShader(SPRITE_SHADER_NAME, SPRITE_SHADER_VERT, SPRITE_SHADER_FRAG)
        || !assetManager.LoadShader(BULLET_SHADER_NAME, BULLET_SHADER_VERT, BULLET_SHADER_FRAG))
    {
        SDL_Log("Failed to load shader");
        exit(EXIT_FAILURE);
    }

    assetManager.CreateSpriteVertex();

    if (!assetManager.LoadTexture(PLAYER_TEXTURE_NAME, PLAYER_TEXTURE)
        || !assetManager.LoadTexture(ENEMY_TEXTURE_NAME, ENEMY_TEXTURE))
    {
        SDL_Log("Failed to load texture");
        exit(EXIT_FAILURE);
    }

    assetManager.LoadFont(FONT_NAME, FONT_PATH);
    auto& font       = assetManager.GetFont(FONT_NAME);
    auto fontTexture = font.RenderText("Hello World !!", Font::DEFAULT_COLOR_WHITE, 40);
    assetManager.AddTexture(TITLE, fontTexture);

    // spawn player
    auto player = Player::Spawn(glm::vec2 {Game::WINDOW_WIDTH / 2.0, Game::WINDOW_HEIGHT / 2.0});

    // spawn enemy
    std::vector<glm::vec2> points = std::vector {
        glm::vec2 {100.0f, 100.0f},
        glm::vec2 {800.0f, 100.0f},
        glm::vec2 {800.0f, 600.0f},
        glm::vec2 {100.0f, 600.0f},
    };
    auto enemy = Enemy::Spawn(points);

    SpawnExplosionBullets(enemy->GetComponent<TransformComponent>().position,
                          RED,
                          18,
                          enemy->GetTag(),
                          300.0f);

    RegisterAction(SDL_SCANCODE_W, "UP");
    RegisterAction(SDL_SCANCODE_A, "LEFT");
    RegisterAction(SDL_SCANCODE_S, "DOWN");
    RegisterAction(SDL_SCANCODE_D, "RIGHT");
    RegisterAction(SDL_SCANCODE_SPACE, "SHOOT");
    RegisterAction(SDL_SCANCODE_P, "PAUSE");
    RegisterAction(SDL_SCANCODE_ESCAPE, "QUIT");
}

void ScenePlay::Update(float deltaTime)
{
    Game::GetGame().GetEntityManger().Update();

    if (paused)
        return;

    MoveEntities(deltaTime);
    ProcessLifespan(deltaTime);
    ProcessCollision();
}

void ScenePlay::OnEnd()
{
    auto& game          = Game::GetGame();
    auto& assetManager  = game.GetAssetManager();
    auto& entityManager = game.GetEntityManger();

    assetManager.RemoveTexture(PLAYER_TEXTURE_NAME);
    assetManager.RemoveTexture(ENEMY_TEXTURE_NAME);
    assetManager.RemoveTexture(TITLE);

    auto& entities = entityManager.GetEntities();
    for (auto& entity : entities)
    {
        entity->Destroy();
    }
    entityManager.Update();

    game.ChangeScene("MENU", std::make_shared<SceneMenu>(), true);
}

void ScenePlay::SetPause(bool pause)
{
    paused = pause;
}

void ScenePlay::SpawnDirectionalBullet(const glm::vec2& position,
                                       const glm::vec2& velocity,
                                       const glm::vec3& color,
                                       const std::string& ownerTag,
                                       const float size)
{
    auto& entityManager = Game::GetGame().GetEntityManger();
    auto bullet         = entityManager.AddEntity("bullet");
    std::vector<std::string> tags {bullet->GetTag(), ownerTag};
    bullet->AddComponent<TransformComponent>(position, velocity);
    bullet->AddComponent<DrawComponent>(BULLET_SHADER_NAME, color);
    bullet->AddComponent<RectComponent>(size);
    bullet->AddComponent<LifespanComponent>(3.0f);
    bullet->AddComponent<BoxCollisionComponent>(glm::vec2 {size / 2.0f, size / 2.0f}, tags);
}

void ScenePlay::SpawnExplosionBullets(const glm::vec2& position,
                                      const glm::vec3& color,
                                      const int bulletsNum,
                                      const std::string& ownerTag,
                                      const float speed,
                                      const float size)
{
    auto& entityManager = Game::GetGame().GetEntityManger();

    float degree        = 360.0f / bulletsNum;
    float currentDegree = 0.0f;
    for (int i = 0; i < bulletsNum; ++i)
    {
        float radian = currentDegree / 180.0f * PI;
        glm::vec2 velocity {0.0f, 0.0f};
        velocity.x = std::cosf(radian) * speed;
        velocity.y = std::sinf(radian) * speed;
        currentDegree += degree;

        auto bullet = entityManager.AddEntity("bullet");
        std::vector<std::string> tags {bullet->GetTag(), ownerTag};
        bullet->AddComponent<TransformComponent>(position, velocity);
        bullet->AddComponent<DrawComponent>(BULLET_SHADER_NAME, color);
        bullet->AddComponent<RectComponent>(size);
        bullet->AddComponent<LifespanComponent>(10.0f);
        bullet->AddComponent<BoxCollisionComponent>(glm::vec2 {size / 2.0f, size / 2.0f}, tags);
    }
}

void ScenePlay::DoAction(const Action& action)
{
    if (action.type == "START")
    {
        if (action.name == "QUIT")
        {
            OnEnd();
        }
        else if (action.name == "PAUSE" && Player::NeedsPause())
        {
            SetPause(!paused);
        }
    }

    Player::ProcessAction(action);
}

void ScenePlay::MoveEntities(float deltaTime)
{
    // player
    Player::Move(deltaTime);

    auto& entityManager = Game::GetGame().GetEntityManger();

    // bullet
    auto& player          = entityManager.GetEntities("player")[0];  // FIXME
    auto& input           = player->GetComponent<InputComponent>();
    auto& playerTransform = player->GetComponent<TransformComponent>();
    input.shootInterval -= deltaTime;
    if (input.shoot && input.shootInterval <= 0.0f)
    {
        SpawnDirectionalBullet(playerTransform.position + glm::vec2 {0.0f, -50.0f},
                               glm::vec2 {0.0f, -600.0f},
                               GREEN,
                               player->GetTag());
        input.ResetShootInterval();
    }

    auto& bullets = entityManager.GetEntities("bullet");
    for (auto& bullet : bullets)
    {
        auto& transform = bullet->GetComponent<TransformComponent>();
        transform.position += transform.velocity * deltaTime;
    }

    // enemy
    Enemy::Move(deltaTime);
}

void ScenePlay::ProcessLifespan(float deltaTime)
{
    auto& entityManager = Game::GetGame().GetEntityManger();
    for (auto& entity : entityManager.GetEntities())
    {
        if (entity->HasComponent<LifespanComponent>())
        {
            auto& lifespan = entity->GetComponent<LifespanComponent>();
            lifespan.lifespan -= deltaTime;
            if (lifespan.lifespan <= 0.0f)
            {
                entity->Destroy();
            }
        }
    }
}

void ScenePlay::ProcessCollision()
{
    Enemy::Collide();

    if (Player::IsCollide())
    {
        OnEnd();
    }
}

void ScenePlay::Render()
{
    auto& game          = Game::GetGame();
    auto& assetManager  = game.GetAssetManager();
    auto& entityManager = game.GetEntityManger();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Set the clear color to black
    glClear(GL_COLOR_BUFFER_BIT);          // Clear the color buffer

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto& vertexArray = assetManager.GetSpriteVertex();

    // Draw Bullet
    auto& bullets = entityManager.GetEntities("bullet");
    for (auto& bullet : bullets)
    {
        auto& draw      = bullet->GetComponent<DrawComponent>();
        auto& transform = bullet->GetComponent<TransformComponent>();
        float edge      = bullet->GetComponent<RectComponent>().edge;

        auto& bulletShader = assetManager.GetShader(draw.shaderName);
        bulletShader.SetActive();
        vertexArray.SetActive();
        bulletShader.SetVector2Uniform("uWindowSize", Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
        bulletShader.SetVector2Uniform("uBulletPosition", transform.position);
        bulletShader.SetVector2Uniform("uBulletSize", edge, edge);
        bulletShader.SetVector3Uniform("uBulletColor", draw.color);
        glDrawElements(GL_TRIANGLES, vertexArray.GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    }

    // draw enemy
    Enemy::Draw();

    // draw player
    Player::Draw();

    auto& spriteShader = assetManager.GetShader(SPRITE_SHADER_NAME);

    // draw text
    auto& fontTexture = assetManager.GetTexture(TITLE);
    spriteShader.SetVector2Uniform("uTextureSize", fontTexture.GetWidth(), fontTexture.GetHeight());
    spriteShader.SetVector2Uniform("uTexturePosition",
                                   glm::vec2 {Game::WINDOW_WIDTH / 2.0f, fontTexture.GetHeight()});
    spriteShader.SetFloatUniform("uTextureScale", 3.0f);
    fontTexture.SetActive();
    glDrawElements(GL_TRIANGLES, vertexArray.GetNumIndices(), GL_UNSIGNED_INT, nullptr);

    // swap the buffers
    SDL_GL_SwapWindow(game.GetWindow());
}
