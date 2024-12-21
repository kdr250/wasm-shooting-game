#include "HealthBar.h"
#include "../Game.h"

int HealthBar::health    = 0;
int HealthBar::maxHealth = 0;

void HealthBar::Spawn(const int maxHP, const glm::vec2& position)
{
    maxHealth = maxHP;
    health    = maxHP;

    auto& game          = Game::GetGame();
    auto& assetManager  = game.GetAssetManager();
    auto& entityManager = game.GetEntityManger();

    if (!assetManager.LoadShader(SPRITE_SHADER_NAME, SPRITE_SHADER_VERT, SPRITE_SHADER_FRAG))
    {
        SDL_Log("Failed to load shader");
        exit(EXIT_FAILURE);
    }

    assetManager.CreateSpriteVertex();

    if (!assetManager.LoadTexture(HEALTH_BAR_TEXTURE_NAME, HEALTH_BAR_TEXTURE_PATH))
    {
        SDL_Log("Failed to load texture");
        exit(EXIT_FAILURE);
    }

    auto healthBar = entityManager.AddEntity(HEALTH_BAR_TAG);
    healthBar->AddComponent<TransformComponent>(position);
    healthBar->AddComponent<SpriteComponent>(SPRITE_SHADER_NAME, HEALTH_BAR_TEXTURE_NAME);
}

void HealthBar::Damaged(int damage)
{
    health -= damage;
}

void HealthBar::Draw()
{
    auto& assetManager = Game::GetGame().GetAssetManager();

    auto& healthBar = GetHealthBar();

    auto& transform = healthBar->GetComponent<TransformComponent>();
    auto& sprite    = healthBar->GetComponent<SpriteComponent>();

    auto& spriteShader = assetManager.GetShader(sprite.shaderName);
    auto& texture      = assetManager.GetTexture(sprite.textureName);
    auto& vertexArray  = assetManager.GetSpriteVertex();

    spriteShader.SetActive();
    vertexArray.SetActive();

    spriteShader.SetVector2Uniform("uWindowSize", Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
    spriteShader.SetVector2Uniform("uTextureSize", health, 50.0f);
    spriteShader.SetVector2Uniform("uTexturePosition", transform.position);
    spriteShader.SetFloatUniform("uTextureScale", 1.0f);

    texture.SetActive();
    glDrawElements(GL_TRIANGLES, vertexArray.GetNumIndices(), GL_UNSIGNED_INT, nullptr);
}

void HealthBar::Unload()
{
    auto& assetManager = Game::GetGame().GetAssetManager();
    auto& healthBar    = GetHealthBar();
    auto& sprite       = healthBar->GetComponent<SpriteComponent>();
    assetManager.RemoveTexture(sprite.textureName);
}

std::shared_ptr<Entity>& HealthBar::GetHealthBar()
{
    auto& entityManager = Game::GetGame().GetEntityManger();
    return entityManager.GetEntities(HEALTH_BAR_TAG)[0];
}
