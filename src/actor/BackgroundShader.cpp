#include "BackgroundShader.h"
#include "../Game.h"

void BackgroundShader::Spawn()
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

    entityManager.AddEntity(BACKGROUND_SHADER_TAG);
}

void BackgroundShader::Draw()
{
    auto& game          = Game::GetGame();
    auto& assetManager  = game.GetAssetManager();
    auto& entityManager = game.GetEntityManger();

    auto& background = GetBackgroundShader();

    auto& spriteShader = assetManager.GetShader(BACKGROUND_SHADER_NAME);
    auto& vertexArray  = assetManager.GetSpriteVertex();

    spriteShader.SetActive();
    vertexArray.SetActive();

    spriteShader.SetVector2Uniform("uWindowSize", Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
    spriteShader.SetVector3Uniform("uColor", glm::vec3(0.2118, 0.8941, 0.8941));  // FIXME
    spriteShader.SetFloatUniform("uTime", game.SceneElapsedTimeMillisecond() / 1000.0f);

    glDrawElements(GL_TRIANGLES, vertexArray.GetNumIndices(), GL_UNSIGNED_INT, nullptr);
}

const std::shared_ptr<Entity>& BackgroundShader::GetBackgroundShader()
{
    auto& entityManager = Game::GetGame().GetEntityManger();
    return entityManager.GetEntities(BACKGROUND_SHADER_TAG)[0];
}
