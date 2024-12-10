#include "ScenePlay.h"
#include <SDL2/SDL_scancode.h>
#include <algorithm>
#include <cmath>
#include "../Game.h"
#include "../actor/Bullet.h"
#include "../actor/Enemy.h"
#include "../actor/Player.h"
#include "../actor/TextActor.h"
#include "Action.h"
#include "SceneMenu.h"

ScenePlay::ScenePlay(const int sceneId) : Scene(sceneId)
{
    TextActor::Spawn("Hello World !!", glm::vec2 {Game::WINDOW_WIDTH / 2.0f, 50.0f});

    Player::Spawn(glm::vec2 {Game::WINDOW_WIDTH / 2.0, Game::WINDOW_HEIGHT / 2.0});

    std::vector<glm::vec2> points = {
        glm::vec2 {200.0f, 200.0f},
        // glm::vec2 {100.0f, 100.0f},
        // glm::vec2 {800.0f, 100.0f},
        // glm::vec2 {800.0f, 600.0f},
        // glm::vec2 {100.0f, 600.0f},
    };
    auto enemy = Enemy::Spawn(points);

    std::vector<std::function<bool(long, int)>> events = {
        [enemy](long fromPreviousMilli, int executionCount)
        {
            long fromPreviousSecond = fromPreviousMilli / 1000;
            if (executionCount < 1 && fromPreviousSecond >= 1)
            {
                Bullet::SpawnExplosionBullets(enemy->GetComponent<TransformComponent>().position,
                                              Bullet::RED,
                                              18,               // number of bullets
                                              enemy->GetTag(),  // owner tag
                                              300.0f,           // speed
                                              200.0f            // size
                );
                return true;
            }
            return false;
        },
        [enemy](long fromPreviousMilli, int executionCount)
        {
            long fromPreviousSecond = fromPreviousMilli / 1000;
            auto& transform         = enemy->GetComponent<TransformComponent>();
            int divide              = 36;
            int bulletsNum          = divide * 2;
            float degree            = 360.0f / divide;
            float currentDegree     = degree * executionCount;
            float radian            = currentDegree / 180.0f * Bullet::PI;
            float speed             = 200.0f;
            glm::vec2 velocity {std::cosf(radian) * speed, std::sinf(radian) * speed};
            if ((executionCount == 0 && fromPreviousSecond >= 5)
                || (executionCount > 0 && executionCount < bulletsNum && fromPreviousMilli >= 50))
            {
                Bullet::SpawnDirectionalBullet(transform.position,  // position
                                               velocity,            // velocity
                                               Bullet::RED,         // color
                                               enemy->GetTag(),     // owner tag
                                               200.0f               // size
                );
                return true;
            }
            return false;
        }};

    enemy->AddComponent<EventComponent>(events);

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

    Player::Unload();
    Enemy::Unload();
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
        OnEnd();
    }
}

void ScenePlay::Render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Set the clear color to black
    glClear(GL_COLOR_BUFFER_BIT);          // Clear the color buffer

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Bullet::Draw();
    Enemy::Draw();
    Player::Draw();
    TextActor::Draw();

    // swap the buffers
    SDL_GL_SwapWindow(Game::GetGame().GetWindow());
}
