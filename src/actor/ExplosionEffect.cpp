#include "ExplosionEffect.h"
#include "../Game.h"

void ExplosionEffect::Spawn(const glm::vec2& position)
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

    if (!assetManager.LoadAnimation(EXPLOSION_NAME, EXPLOSION_PATH, glm::vec2 {16.0f, 16.0f}, 3))
    {
        SDL_Log("Failed to load animation");
        exit(EXIT_FAILURE);
    }

    auto explosion = entityManager.AddEntity(EXPLOSION_TAG);
    explosion->AddComponent<SpriteComponent>(SPRITE_SHADER_NAME, EXPLOSION_NAME);
    explosion->AddComponent<TransformComponent>(position);
    explosion->AddComponent<LifespanComponent>(0.1f);
}

void ExplosionEffect::Draw()
{
    auto& game         = Game::GetGame();
    auto& assetManager = game.GetAssetManager();
    auto& vertexArray  = assetManager.GetSpriteVertex();

    for (auto& explosions : GetExplosionEffects())
    {
        auto& sprite    = explosions->GetComponent<SpriteComponent>();
        auto& transform = explosions->GetComponent<TransformComponent>();

        auto& spriteShader = assetManager.GetShader(sprite.shaderName);
        auto& animation    = assetManager.GetAnimation(sprite.textureName);

        spriteShader.SetActive();
        spriteShader.SetVector2Uniform("uWindowSize", Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
        spriteShader.SetVector2Uniform("uTextureSize", animation.GetSize());
        spriteShader.SetVector2Uniform("uTexturePosition", transform.position);
        spriteShader.SetFloatUniform("uTextureScale", 20.0f);
        animation.Update();
        animation.SetActive();

        glDrawElements(GL_TRIANGLES, vertexArray.GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    }
}

void ExplosionEffect::Unload()
{
    auto& assetManager = Game::GetGame().GetAssetManager();
    assetManager.RemoveAnimation(EXPLOSION_NAME);
}

const std::vector<std::shared_ptr<Entity>>& ExplosionEffect::GetExplosionEffects()
{
    auto& entityManager = Game::GetGame().GetEntityManger();
    return entityManager.GetEntities(EXPLOSION_TAG);
}
