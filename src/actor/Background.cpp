#include "Background.h"
#include "../Game.h"

void Background::Spawn()
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

    if (!assetManager.LoadTexture(BACKGROUND_NAME_ONE, BACKGROUND_PATH_ONE)
        || !assetManager.LoadTexture(BACKGROUND_NAME_TWO, BACKGROUND_PATH_TWO))
    {
        SDL_Log("Failed to load texture");
        exit(EXIT_FAILURE);
    }

    auto background1 = entityManager.AddEntity(BACKGROUND_TAG);
    background1->AddComponent<SpriteComponent>(SPRITE_SHADER_NAME, BACKGROUND_NAME_ONE);
    background1->AddComponent<TransformComponent>(
        glm::vec2 {Game::WINDOW_WIDTH / 2.0f, Game::WINDOW_HEIGHT / 2.0f},
        glm::vec2 {0.0f, 0.5f},
        4.0f);

    auto background2 = entityManager.AddEntity(BACKGROUND_TAG);
    background2->AddComponent<SpriteComponent>(SPRITE_SHADER_NAME, BACKGROUND_NAME_TWO);
    background2->AddComponent<TransformComponent>(
        glm::vec2 {Game::WINDOW_WIDTH / 2.0f, Game::WINDOW_HEIGHT / -2.0f},
        glm::vec2 {0.0f, 0.5f},
        4.0f);
}

void Background::Draw()
{
    auto& game          = Game::GetGame();
    auto& assetManager  = game.GetAssetManager();
    auto& entityManager = game.GetEntityManger();

    for (auto& background : GetBackgrounds())
    {
        auto& sprite    = background->GetComponent<SpriteComponent>();
        auto& transform = background->GetComponent<TransformComponent>();
        transform.position += transform.velocity;
        if (transform.position.y >= Game::WINDOW_HEIGHT * 1.5f)
        {
            transform.position.y = Game::WINDOW_HEIGHT / -2.0f;
            transform.position += transform.velocity;
        }

        auto& texture      = assetManager.GetTexture(sprite.textureName);
        auto& spriteShader = assetManager.GetShader(sprite.shaderName);
        auto& vertexArray  = assetManager.GetSpriteVertex();

        spriteShader.SetActive();
        vertexArray.SetActive();

        spriteShader.SetVector2Uniform("uWindowSize", Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
        spriteShader.SetVector2Uniform("uTextureSize", texture.GetWidth(), texture.GetHeight());
        spriteShader.SetVector2Uniform("uTexturePosition", transform.position);
        spriteShader.SetFloatUniform("uTextureScale", transform.scale);

        texture.SetActive();

        glDrawElements(GL_TRIANGLES, vertexArray.GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    }
}

const std::vector<std::shared_ptr<Entity>>& Background::GetBackgrounds()
{
    auto& entityManager = Game::GetGame().GetEntityManger();
    return entityManager.GetEntities(BACKGROUND_TAG);
}
