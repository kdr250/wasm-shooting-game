#include "ScenePlay.h"
#include <SDL2/SDL_scancode.h>
#include <algorithm>
#include <cmath>
#include "../Game.h"
#include "../actor/Background.h"
#include "../actor/Bullet.h"
#include "../actor/BulletSpawner.h"
#include "../actor/Enemy.h"
#include "../actor/EnemySpawner.h"
#include "../actor/ExplosionEffect.h"
#include "../actor/Player.h"
#include "../actor/PlayerLife.h"
#include "../actor/ScoreActor.h"
#include "../actor/TextActor.h"
#include "Action.h"
#include "SceneMenu.h"

ScenePlay::ScenePlay(const int sceneId) : Scene(sceneId)
{
    Player::Spawn(glm::vec2 {Game::WINDOW_WIDTH / 2.0, Game::WINDOW_HEIGHT / 2.0});
    Background::Spawn();
    BulletSpawner::Initialize();
    EnemySpawner::Initialize(sceneId);
    ScoreActor::Spawn(glm::vec2 {Game::WINDOW_WIDTH - 200.0f, Game::WINDOW_HEIGHT - 50.0f});

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

    if (paused || cleared || over)
        return;

    SpawnEntities(deltaTime);
    MoveEntities(deltaTime);
    ProcessLifespan(deltaTime);
    ProcessCollision();
}

void ScenePlay::OnEnd()
{
    auto& game          = Game::GetGame();
    auto& assetManager  = game.GetAssetManager();
    auto& entityManager = game.GetEntityManger();

    Player::Unload();
    Enemy::Unload();
    ExplosionEffect::Unload();
    TextActor::Unload();

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

void ScenePlay::DoAction(const Action& action)
{
    if (action.type == "START")
    {
        if (action.name == "QUIT")
        {
            OnEnd();
            return;
        }
        else if (action.name == "PAUSE" && Player::NeedsPause())
        {
            SetPause(!paused);
        }
    }

    Player::ProcessAction(action);
}

void ScenePlay::SpawnEntities(float deltaTime)
{
    EnemySpawner::Spawn(deltaTime);
    EnemySpawner::Update(deltaTime);  // FIXME
    BulletSpawner::Spawn(deltaTime);
}

void ScenePlay::MoveEntities(float deltaTime)
{
    Player::Move(deltaTime);
    Bullet::Move(deltaTime);
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
        OnOver();
    }
}

void ScenePlay::Render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Set the clear color to black
    glClear(GL_COLOR_BUFFER_BIT);          // Clear the color buffer

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Background::Draw();
    Bullet::Draw();
    Enemy::Draw();
    ExplosionEffect::Draw();
    Player::Draw();
    TextActor::Draw();
    PlayerLife::Draw();
    ScoreActor::Draw();

    // swap the buffers
    SDL_GL_SwapWindow(Game::GetGame().GetWindow());
}

void ScenePlay::OnClear()
{
    TextActor::Spawn("Game Clear !!",
                     glm::vec2 {Game::WINDOW_WIDTH / 2.0f, Game::WINDOW_HEIGHT / 2.0f});
    cleared = true;
}

void ScenePlay::OnOver()
{
    TextActor::Spawn("Game Over !!",
                     glm::vec2 {Game::WINDOW_WIDTH / 2.0f, Game::WINDOW_HEIGHT / 2.0f});
    over = true;
}
