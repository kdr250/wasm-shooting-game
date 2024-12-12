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

    assetManager.LoadTexture(BACKGROUND_NAME_ONE, BACKGROUND_PATH_ONE);
}

void Background::Draw()
{
    auto& game         = Game::GetGame();
    auto& assetManager = game.GetAssetManager();
    auto& texture      = assetManager.GetTexture(BACKGROUND_NAME_ONE);
    auto& spriteShader = assetManager.GetShader(SPRITE_SHADER_NAME);
    auto& vertexArray  = assetManager.GetSpriteVertex();

    spriteShader.SetActive();
    vertexArray.SetActive();

    spriteShader.SetVector2Uniform("uWindowSize", Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
    spriteShader.SetVector2Uniform("uTextureSize", texture.GetWidth(), texture.GetHeight());
    spriteShader.SetVector2Uniform("uTexturePosition",
                                   Game::WINDOW_WIDTH / 2.0f,
                                   Game::WINDOW_HEIGHT / 2.0f);
    spriteShader.SetFloatUniform("uTextureScale", 2.0f);
    texture.SetActive();

    glDrawElements(GL_TRIANGLES, vertexArray.GetNumIndices(), GL_UNSIGNED_INT, nullptr);
}
