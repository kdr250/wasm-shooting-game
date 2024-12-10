#include "Bullet.h"
#include "../Game.h"

void Bullet::SpawnDirectionalBullet(const glm::vec2& position,
                                    const glm::vec2& velocity,
                                    const glm::vec3& color,
                                    const std::string& ownerTag,
                                    const float size)
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
    auto& game          = Game::GetGame();
    auto& assetManager  = game.GetAssetManager();
    auto& entityManager = game.GetEntityManger();

    if (!assetManager.LoadShader(BULLET_SHADER_NAME, BULLET_SHADER_VERT, BULLET_SHADER_FRAG))
    {
        SDL_Log("Failed to load shader");
        exit(EXIT_FAILURE);
    }

    assetManager.CreateSpriteVertex();

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

void Bullet::Move(float deltaTime)
{
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

    for (auto& bullet : GetBullets())
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
}

const std::vector<std::shared_ptr<Entity>>& Bullet::GetBullets()
{
    auto& entityManager = Game::GetGame().GetEntityManger();
    return entityManager.GetEntities(BULLET_TAG);
}
