#include "Enemy.h"
#include "../Game.h"
#include "Bullet.h"

std::shared_ptr<Entity> Enemy::Spawn(const std::vector<glm::vec2>& movePoints)
{
    auto& game          = Game::GetGame();
    auto& assetManager  = game.GetAssetManager();
    auto& entityManager = game.GetEntityManger();

    if (!assetManager.LoadShader(SPRITE_SHADER_NAME, SPRITE_SHADER_VERT, SPRITE_SHADER_FRAG))
    {
        SDL_Log("Failed to load shader");
        exit(EXIT_FAILURE);
    }

    assetManager.CreateSpriteVertex();

    if (!assetManager.LoadTexture(ENEMY_TEXTURE_NAME, ENEMY_TEXTURE))
    {
        SDL_Log("Failed to load texture");
        exit(EXIT_FAILURE);
    }

    float speed = 200.0f;
    float scale = 10.0f;

    auto enemy = entityManager.AddEntity(ENEMY_TAG);
    enemy->AddComponent<SpriteComponent>(SPRITE_SHADER_NAME, ENEMY_TEXTURE_NAME);
    auto& enemyTexture = assetManager.GetTexture(ENEMY_TEXTURE_NAME);
    enemy->AddComponent<BoxCollisionComponent>(
        glm::vec2 {enemyTexture.GetWidth(), enemyTexture.GetHeight()} * scale / 4.0f,
        std::vector {enemy->GetTag()});
    auto& aiMove = enemy->AddComponent<AIMoveComponent>(movePoints, speed);
    enemy->AddComponent<TransformComponent>(aiMove.CurrentPoint(), scale);

    return enemy;
}

void Enemy::Move(float deltaTime)
{
    for (auto& enemy : GetEnemies())
    {
        if (enemy->HasComponent<EventComponent>())
        {
            auto& eventComponent = enemy->GetComponent<EventComponent>();
            eventComponent.Execute(deltaTime);
        }
    }
}

void Enemy::Collide()
{
    auto& entityManager = Game::GetGame().GetEntityManger();
    auto& enemies       = GetEnemies();

    for (auto& bullet : Bullet::GetBullets())
    {
        for (auto& enemy : enemies)
        {
            if (Physics::IsOverlap(bullet, enemy))
            {
                enemy->Destroy();
                bullet->Destroy();
                continue;
            }
        }
    }
}

void Enemy::Draw()
{
    auto& assetManager = Game::GetGame().GetAssetManager();
    auto& vertexArray  = assetManager.GetSpriteVertex();

    for (auto& enemy : GetEnemies())
    {
        auto& enemyTransform = enemy->GetComponent<TransformComponent>();
        auto& enemySprite    = enemy->GetComponent<SpriteComponent>();

        auto& spriteShader = assetManager.GetShader(enemySprite.shaderName);
        auto& texture      = assetManager.GetTexture(enemySprite.textureName);

        spriteShader.SetActive();
        vertexArray.SetActive();

        spriteShader.SetVector2Uniform("uWindowSize", Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
        spriteShader.SetVector2Uniform("uTextureSize", texture.GetWidth(), texture.GetHeight());
        spriteShader.SetVector2Uniform("uTexturePosition", enemyTransform.position);
        spriteShader.SetFloatUniform("uTextureScale", enemyTransform.scale);

        texture.SetActive();
        glDrawElements(GL_TRIANGLES, vertexArray.GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    }
}

void Enemy::Unload()
{
    auto& assetManager = Game::GetGame().GetAssetManager();
    for (auto& enemy : GetEnemies())
    {
        auto& sprite = enemy->GetComponent<SpriteComponent>();
        assetManager.RemoveTexture(sprite.textureName);
    }
}

const std::vector<std::shared_ptr<Entity>>& Enemy::GetEnemies()
{
    auto& entityManager = Game::GetGame().GetEntityManger();
    return entityManager.GetEntities(ENEMY_TAG);
}
