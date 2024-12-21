#include "Enemy.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include "../Game.h"
#include "Bullet.h"
#include "ExplosionEffect.h"
#include "HealthBar.h"
#include "Player.h"
#include "ScoreActor.h"

void Enemy::Move(float deltaTime)
{
    for (auto& enemy : GetEnemies())
    {
        if (enemy->HasComponent<EventComponent>())
        {
            auto& eventComponent = enemy->GetComponent<EventComponent>();
            eventComponent.Execute(deltaTime);
        }
        else if (enemy->HasComponent<MoveComponent>() && enemy->HasComponent<TransformComponent>())
        {
            auto& move         = enemy->GetComponent<MoveComponent>();
            auto& transform    = enemy->GetComponent<TransformComponent>();
            transform.position = move.Move(deltaTime);
        }
        else if (enemy->HasComponent<SplineMoveComponent>()
                 && enemy->HasComponent<TransformComponent>())
        {
            auto& spline       = enemy->GetComponent<SplineMoveComponent>();
            auto& transform    = enemy->GetComponent<TransformComponent>();
            transform.position = spline.Move(deltaTime);
        }
    }
}

void Enemy::Collide()
{
    auto& entityManager = Game::GetGame().GetEntityManger();
    auto& enemies       = GetEnemies();

    for (auto& bullet : Bullet::GetBullets(Player::PLAYER_TAG))
    {
        for (auto& enemy : enemies)
        {
            if (Physics::IsOverlap(bullet, enemy))
            {
                bullet->Destroy();

                if (enemy->HasComponent<HealthComponent>())
                {
                    auto& health = enemy->GetComponent<HealthComponent>();
                    health.Damaged(100);  // FIXME
                    HealthBar::Damaged(100);
                    if (health.IsAlive())
                    {
                        continue;
                    }
                }

                enemy->Destroy();
                ExplosionEffect::Spawn(enemy->GetComponent<TransformComponent>().position);
                ScoreActor::AddScore(300);
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

        if (enemy->HasComponent<HealthComponent>())
        {
            HealthBar::Draw();
        }
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
