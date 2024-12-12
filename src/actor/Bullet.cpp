#include "Bullet.h"
#include "../Game.h"

void Bullet::Initialize()
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

void Bullet::SpawnDirectionalBullet(const glm::vec2& position,
                                    const glm::vec2& velocity,
                                    const glm::vec3& color,
                                    const std::string& ownerTag,
                                    const float size)
{
    auto& entityManager = Game::GetGame().GetEntityManger();

    auto bullet = entityManager.AddEntity(BULLET_TAG);
    std::vector<std::string> tags {bullet->GetTag(), ownerTag};
    bullet->AddComponent<TransformComponent>(position, velocity);
    bullet->AddComponent<DrawComponent>(BULLET_SHADER_NAME, color);
    bullet->AddComponent<RectComponent>(size);
    bullet->AddComponent<LifespanComponent>(3.0f);
    bullet->AddComponent<BoxCollisionComponent>(glm::vec2 {size / 2.0f, size / 2.0f}, tags);
}

void Bullet::SpawnExplosionBullets(const glm::vec2& position,
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

        auto bullet = entityManager.AddEntity(BULLET_TAG);
        std::vector<std::string> tags {bullet->GetTag(), ownerTag};
        bullet->AddComponent<TransformComponent>(position, velocity);
        bullet->AddComponent<DrawComponent>(BULLET_SHADER_NAME, color);
        bullet->AddComponent<RectComponent>(size);
        bullet->AddComponent<LifespanComponent>(10.0f);
        bullet->AddComponent<BoxCollisionComponent>(glm::vec2 {size / 2.0f, size / 2.0f}, tags);
    }
}

void Bullet::SpawnRollBullets(const glm::vec2& position,
                              const glm::vec3& color,
                              const int bulletsNum,
                              const std::string& ownerTag,
                              const float speed,
                              const float size)
{
    std::function<Result(long, int)> event =
        [position, bulletsNum, speed, color, ownerTag](long fromPreviousMilli, int executionCount)
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
            float radian        = currentDegree / 180.0f * Bullet::PI;
            glm::vec2 velocity {std::cosf(radian) * speed, std::sinf(radian) * speed};

            SpawnDirectionalBullet(position,  // position
                                   velocity,  // velocity
                                   color,     // color
                                   ownerTag,  // owner tag
                                   200.0f     // size
            );
            return Result::CONTINUE;
        }
        return Result::NONE;
    };

    auto spawner         = GetSpawner();
    auto& eventComponent = spawner->GetComponent<EventComponent>();
    eventComponent.Add(event);
}

void Bullet::SpawnSequentialBullets(const glm::vec2& position,
                                    const glm::vec2& target,
                                    const int bulletsNum,
                                    const glm::vec3& color,
                                    const std::string& ownerTag,
                                    const float speed,
                                    const float size)
{
    std::function<Result(long, int)> event =
        [position, target, bulletsNum, speed, color, ownerTag](long fromPreviousMilli,
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
            Bullet::SpawnDirectionalBullet(position,  // position
                                           velocity,  // velocity
                                           color,     // color
                                           ownerTag,  // owner tag
                                           200.0f     // size
            );
            return Result::CONTINUE;
        }
        return Result::NONE;
    };

    auto spawner         = GetSpawner();
    auto& eventComponent = spawner->GetComponent<EventComponent>();
    eventComponent.Add(event);
}

void Bullet::SpawnWinderBullets(const glm::vec2& position,
                                const glm::vec2& target,
                                const int loopNum,
                                const glm::vec3& color,
                                const std::string& ownerTag,
                                const float speed,
                                const float size)
{
    std::function<Result(long, int)> event =
        [position, target, loopNum, speed, color, ownerTag](long fromPreviousMilli,
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
                float radianToAdd =
                    baseRadian + (executionCount + i * 30 + 15) / 180.0f * Bullet::PI;
                glm::vec2 velocity {std::cosf(radianToAdd) * speed, std::sinf(radianToAdd) * speed};
                Bullet::SpawnDirectionalBullet(position,  // position
                                               velocity,  // velocity
                                               color,     // color
                                               ownerTag,  // owner tag
                                               200.0f     // size
                );
            }
            return Result::CONTINUE;
        }
        return Result::NONE;
    };

    auto spawner         = GetSpawner();
    auto& eventComponent = spawner->GetComponent<EventComponent>();
    eventComponent.Add(event);
}

void Bullet::Move(float deltaTime)
{
    auto spawner         = GetSpawner();
    auto& eventComponent = spawner->GetComponent<EventComponent>();
    eventComponent.Execute(deltaTime);

    for (auto& bullet : GetBullets())
    {
        auto& transform = bullet->GetComponent<TransformComponent>();
        transform.position += transform.velocity * deltaTime;
    }
}

void Bullet::Draw()
{
    auto& assetManager = Game::GetGame().GetAssetManager();
    auto& vertexArray  = assetManager.GetSpriteVertex();

    auto& bullets = GetBullets();

    if (bullets.empty())
    {
        return;
    }

    std::vector<glm::vec2> positions;
    std::vector<glm::vec2> sizes;
    std::vector<glm::vec3> colors;
    for (auto& bullet : bullets)
    {
        auto& draw      = bullet->GetComponent<DrawComponent>();
        auto& transform = bullet->GetComponent<TransformComponent>();
        float edge      = bullet->GetComponent<RectComponent>().edge;

        positions.emplace_back(transform.position);
        sizes.emplace_back(glm::vec2 {edge, edge});
        colors.emplace_back(draw.color);
    }

    auto& bulletShader = assetManager.GetShader(BULLET_SHADER_NAME);
    bulletShader.SetActive();
    vertexArray.SetActive();
    bulletShader.SetVector2Uniform("uWindowSize", Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
    bulletShader.SetIntUniform("uBulletCount", bullets.size());
    bulletShader.SetVector2Uniforms("uBulletPositions", positions);
    bulletShader.SetVector2Uniforms("uBulletSizes", sizes);
    bulletShader.SetVector3Uniforms("uBulletColors", colors);
    glDrawElements(GL_TRIANGLES, vertexArray.GetNumIndices(), GL_UNSIGNED_INT, nullptr);
}

const std::vector<std::shared_ptr<Entity>>& Bullet::GetBullets()
{
    auto& entityManager = Game::GetGame().GetEntityManger();
    return entityManager.GetEntities(BULLET_TAG);
}

std::shared_ptr<Entity> Bullet::GetSpawner()
{
    auto& entityManager = Game::GetGame().GetEntityManger();
    if (entityManager.HasEntities(SPAWNER_TAG))
    {
        return entityManager.GetEntities(SPAWNER_TAG)[0];
    }

    auto spawner = entityManager.AddEntity(SPAWNER_TAG);
    spawner->AddComponent<EventComponent>();

    return spawner;
}
