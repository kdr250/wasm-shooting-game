#include "TextActor.h"
#include "../Game.h"

void TextActor::Spawn(const std::string& text, const glm::vec2& position)
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

    assetManager.LoadFont(FONT_NAME, FONT_PATH);
    auto& font              = assetManager.GetFont(FONT_NAME);
    auto texture            = font.RenderText(text, Font::DEFAULT_COLOR_WHITE, 40);
    std::string textureName = "text_" + text;
    assetManager.AddTexture(textureName, texture);

    auto textActor = entityManager.AddEntity(TEXT_TAG);
    textActor->AddComponent<SpriteComponent>(SPRITE_SHADER_NAME, textureName);
    textActor->AddComponent<TransformComponent>(position);
}

void TextActor::Draw()
{
    auto& game          = Game::GetGame();
    auto& assetManager  = game.GetAssetManager();
    auto& entityManager = game.GetEntityManger();

    auto& vertexArray = assetManager.GetSpriteVertex();

    for (auto& textActor : entityManager.GetEntities(TEXT_TAG))
    {
        auto& sprite    = textActor->GetComponent<SpriteComponent>();
        auto& transform = textActor->GetComponent<TransformComponent>();

        auto& spriteShader = assetManager.GetShader(sprite.shaderName);
        auto& texture      = assetManager.GetTexture(sprite.textureName);

        spriteShader.SetActive();
        spriteShader.SetVector2Uniform("uWindowSize", Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
        spriteShader.SetVector2Uniform("uTextureSize", texture.GetWidth(), texture.GetHeight());
        spriteShader.SetVector2Uniform("uTexturePosition", transform.position);
        spriteShader.SetFloatUniform("uTextureScale", 3.0f);
        texture.SetActive();

        glDrawElements(GL_TRIANGLES, vertexArray.GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    }
}
