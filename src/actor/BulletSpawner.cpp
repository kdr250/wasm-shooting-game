#include "BulletSpawner.h"
#include "../Game.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Player.h"

void BulletSpawner::Initialize()
{
    auto& game          = Game::GetGame();
    auto& assetManager  = game.GetAssetManager();
    auto& entityManager = game.GetEntityManger();

    if (!assetManager.LoadShader(BULLET_SHADER_NAME, BULLET_SHADER_VERT, BULLET_SHADER_FRAG))
    {
        SDL_Log("Failed to load shader");
        exit(EXIT_FAILURE);
    }

    assetManager.CreateSpriteVertex();
}

void BulletSpawner::Spawn(float deltaTime)
{
    for (auto& ownerTag : {Player::PLAYER_TAG, Enemy::ENEMY_TAG})
    {
        auto spawner         = GetSpawner(ownerTag);
        auto& eventComponent = spawner->GetComponent<EventComponent>();
        eventComponent.Execute(deltaTime);
    }
}

void BulletSpawner::RegisterSpawnDirectionalBullet(const glm::vec2& position,
                                                   const glm::vec2& velocity,
                                                   const glm::vec3& color,
                                                   const std::string& ownerTag,
                                                   const float size)
{
    std::function<Result(long, int)> event =
        [position, velocity, color, ownerTag, size](long fromPreviousMilli, int executionCount)
    {
        SpawnDirectionalBullet(position,  // position
                               velocity,  // velocity
                               color,     // color
                               ownerTag,  // owner tag
                               size       // size
        );
        return Result::COMPLETED;
    };

    auto spawner         = GetSpawner(ownerTag);
    auto& eventComponent = spawner->GetComponent<EventComponent>();
    eventComponent.Add(event);
}

void BulletSpawner::RegisterSpawnExplosionBullets(const glm::vec2& position,
                                                  const glm::vec3& color,
                                                  const int bulletsNum,
                                                  const std::string& ownerTag,
                                                  const float speed,
                                                  const float size)
{
    std::function<Result(long, int)> event =
        [position, color, bulletsNum, ownerTag, speed, size](long fromPreviousMilli,
                                                             int executionCount)
    {
        SpawnExplosionBullets(position,    // position
                              color,       // color
                              bulletsNum,  // num of bullets
                              ownerTag,    // owner tag
                              speed,       // speed
                              size         // size
        );
        return Result::COMPLETED;
    };

    auto spawner         = GetSpawner(ownerTag);
    auto& eventComponent = spawner->GetComponent<EventComponent>();
    eventComponent.Add(event);
}

void BulletSpawner::SpawnRollBullets(const glm::vec2& position,
                                     const glm::vec3& color,
                                     const int bulletsNum,
                                     const std::string& ownerTag,
                                     const float speed,
                                     const float size)
{
    std::function<Result(long, int)> event =
        [position, bulletsNum, speed, color, ownerTag, size](long fromPreviousMilli,
                                                             int executionCount)
    {
        if (executionCount == bulletsNum)
        {
            return Result::COMPLETED;
        }
        if (executionCount < bulletsNum && fromPreviousMilli >= 50)
        {
            int divide          = bulletsNum / 2;
            float degree        = 360.0f / divide;
            float currentDegree = degree * executionCount;
            float radian        = currentDegree / 180.0f * PI;
            glm::vec2 velocity {std::cosf(radian) * speed, std::sinf(radian) * speed};

            SpawnDirectionalBullet(position,  // position
                                   velocity,  // velocity
                                   color,     // color
                                   ownerTag,  // owner tag
                                   size       // size
            );
            return Result::CONTINUE;
        }
        return Result::NONE;
    };

    auto spawner         = GetSpawner(ownerTag);
    auto& eventComponent = spawner->GetComponent<EventComponent>();
    eventComponent.Add(event);
}

void BulletSpawner::SpawnSequentialBullets(const glm::vec2& position,
                                           const glm::vec2& target,
                                           const int bulletsNum,
                                           const glm::vec3& color,
                                           const std::string& ownerTag,
                                           const float speed,
                                           const float size)
{
    std::function<Result(long, int)> event =
        [position, target, bulletsNum, speed, color, ownerTag, size](long fromPreviousMilli,
                                                                     int executionCount)
    {
        if (executionCount == bulletsNum)
        {
            return Result::COMPLETED;
        }
        if (executionCount < bulletsNum && fromPreviousMilli >= 50)
        {
            glm::vec2 positionToTarget = target - position;
            glm::vec2 velocity         = glm::normalize(positionToTarget) * speed;
            SpawnDirectionalBullet(position,  // position
                                   velocity,  // velocity
                                   color,     // color
                                   ownerTag,  // owner tag
                                   size       // size
            );
            return Result::CONTINUE;
        }
        return Result::NONE;
    };

    auto spawner         = GetSpawner(ownerTag);
    auto& eventComponent = spawner->GetComponent<EventComponent>();
    eventComponent.Add(event);
}

void BulletSpawner::SpawnWinderBullets(const glm::vec2& position,
                                       const glm::vec2& target,
                                       const int loopNum,
                                       const glm::vec3& color,
                                       const std::string& ownerTag,
                                       const float speed,
                                       const float size)
{
    std::function<Result(long, int)> event =
        [position, target, loopNum, speed, color, ownerTag, size](long fromPreviousMilli,
                                                                  int executionCount)
    {
        if (executionCount == loopNum)
        {
            return Result::COMPLETED;
        }
        if (executionCount < loopNum && fromPreviousMilli >= 50)
        {
            glm::vec2 positionToTarget = target - position;
            glm::vec2 normalized       = glm::normalize(positionToTarget);
            float baseRadian           = std::atan2f(normalized.y, normalized.x);
            for (int i = -3; i < 3; ++i)
            {
                float radianToAdd = baseRadian + (executionCount + i * 30 + 15) / 180.0f * PI;
                glm::vec2 velocity {std::cosf(radianToAdd) * speed, std::sinf(radianToAdd) * speed};
                SpawnDirectionalBullet(position,  // position
                                       velocity,  // velocity
                                       color,     // color
                                       ownerTag,  // owner tag
                                       size       // size
                );
            }
            return Result::CONTINUE;
        }
        return Result::NONE;
    };

    auto spawner         = GetSpawner(ownerTag);
    auto& eventComponent = spawner->GetComponent<EventComponent>();
    eventComponent.Add(event);
}

void BulletSpawner::SpawnDirectionalBullet(const glm::vec2& position,
                                           const glm::vec2& velocity,
                                           const glm::vec3& color,
                                           const std::string& ownerTag,
                                           const float size)
{
    auto& entityManager = Game::GetGame().GetEntityManger();

    std::string tag = Bullet::GenerateTagName(ownerTag);

    auto bullet = entityManager.AddEntity(tag);
    std::vector<std::string> excludes {bullet->GetTag(), ownerTag};
    bullet->AddComponent<TransformComponent>(position, velocity);
    bullet->AddComponent<DrawComponent>(BULLET_SHADER_NAME, color);
    bullet->AddComponent<RectComponent>(size);
    bullet->AddComponent<LifespanComponent>(3.0f);
    bullet->AddComponent<BoxCollisionComponent>(glm::vec2 {size / 2.0f, size / 2.0f}, excludes);
}

void BulletSpawner::SpawnExplosionBullets(const glm::vec2& position,
                                          const glm::vec3& color,
                                          const int bulletsNum,
                                          const std::string& ownerTag,
                                          const float speed,
                                          const float size)
{
    auto& entityManager = Game::GetGame().GetEntityManger();

    std::string tag = Bullet::GenerateTagName(ownerTag);

    float degree        = 360.0f / bulletsNum;
    float currentDegree = 0.0f;
    for (int i = 0; i < bulletsNum; ++i)
    {
        float radian = currentDegree / 180.0f * PI;
        glm::vec2 velocity {0.0f, 0.0f};
        velocity.x = std::cosf(radian) * speed;
        velocity.y = std::sinf(radian) * speed;
        currentDegree += degree;

        auto bullet = entityManager.AddEntity(tag);
        std::vector<std::string> excludes {bullet->GetTag(), ownerTag};
        bullet->AddComponent<TransformComponent>(position, velocity);
        bullet->AddComponent<DrawComponent>(BULLET_SHADER_NAME, color);
        bullet->AddComponent<RectComponent>(size);
        bullet->AddComponent<LifespanComponent>(10.0f);
        bullet->AddComponent<BoxCollisionComponent>(glm::vec2 {size / 2.0f, size / 2.0f}, excludes);
    }
}

std::shared_ptr<Entity> BulletSpawner::GetSpawner(const std::string& ownerTag)
{
    auto& entityManager    = Game::GetGame().GetEntityManger();
    std::string spawnerTag = GenerateSpawnerTagName(ownerTag);
    if (entityManager.HasEntities(spawnerTag))
    {
        return entityManager.GetEntities(spawnerTag)[0];
    }

    auto spawner = entityManager.AddEntity(spawnerTag);
    spawner->AddComponent<EventComponent>();

    return spawner;
}

std::string BulletSpawner::GenerateSpawnerTagName(const std::string& ownerTag)
{
    return SPAWNER_TAG + "_" + ownerTag;
}
