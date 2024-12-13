#include "Background.h"
#include "../Game.h"

void Background::Spawn()
{
    auto& game          = Game::GetGame();
    auto& assetManager  = game.GetAssetManager();
    auto& entityManager = game.GetEntityManger();

    if (!assetManager.LoadShader(BACKGROUND_SHADER_NAME,
                                 BACKGROUND_SHADER_VERT,
                                 BACKGROUND_SHADER_FRAG))
    {
        SDL_Log("Failed to load shader");
        exit(EXIT_FAILURE);
    }

    assetManager.CreateSpriteVertex();

    if (!assetManager.LoadTexture(BACKGROUND_NAME_ONE, BACKGROUND_PATH_ONE))
    {
        SDL_Log("Failed to load texture");
        exit(EXIT_FAILURE);
    }
}

void Background::Draw()
{
    auto& game             = Game::GetGame();
    auto& assetManager     = game.GetAssetManager();
    auto& texture          = assetManager.GetTexture(BACKGROUND_NAME_ONE);
    auto& backgroundShader = assetManager.GetShader(BACKGROUND_SHADER_NAME);
    auto& vertexArray      = assetManager.GetSpriteVertex();

    backgroundShader.SetActive();
    vertexArray.SetActive();
    texture.SetActive();

    glDrawElements(GL_TRIANGLES, vertexArray.GetNumIndices(), GL_UNSIGNED_INT, nullptr);
}
