#include "ScenePlay.h"
#include <SDL2/SDL_scancode.h>
#include "../Game.h"
#include "Action.h"

ScenePlay::ScenePlay(const int sceneId) : id(sceneId)
{
    auto& game          = Game::GetGame();
    auto& assetManager  = game.GetAssetManager();
    auto& entityManager = game.GetEntityManger();

    std::string shaderName  = "sprite";
    std::string textureName = "example";

    if (!assetManager.LoadShader(shaderName, SPRITE_SHADER_VERT, SPRITE_SHADER_FRAG))
    {
        SDL_Log("Failed to load shader");
        exit(EXIT_FAILURE);
    }

    assetManager.CreateSpriteVertex();

    if (!assetManager.LoadTexture(textureName, PLAYER_TEXTURE))
    {
        SDL_Log("Failed to load texture");
        exit(EXIT_FAILURE);
    }

    player = entityManager.AddEntity("player");
    player->AddComponent<StateComponent>();
    player->AddComponent<TransformComponent>(
        glm::vec2 {Game::WINDOW_WIDTH / 2.0, Game::WINDOW_HEIGHT / 2.0},  // position
        10.0f,                                                            // scale
        200.0f                                                            // speed
    );
    player->AddComponent<InputComponent>();
    player->AddComponent<SpriteComponent>(shaderName, textureName);

    RegisterAction(SDL_SCANCODE_W, "UP");
    RegisterAction(SDL_SCANCODE_A, "LEFT");
    RegisterAction(SDL_SCANCODE_S, "DOWN");
    RegisterAction(SDL_SCANCODE_D, "RIGHT");
    RegisterAction(SDL_SCANCODE_P, "PAUSE");
    RegisterAction(SDL_SCANCODE_ESCAPE, "QUIT");
}

void ScenePlay::Update(float deltaTime)
{
    Game::GetGame().GetEntityManger().Update();

    MoveEntities(deltaTime);
    Render();
}

void ScenePlay::OnEnd()
{
    // FIXME
    Game::GetGame().Stop();
}

void ScenePlay::DoAction(const Action& action)
{
    auto& input = player->GetComponent<InputComponent>();
    input.Reset();

    if (action.GetType() == "START")
    {
        if (action.GetName() == "QUIT")
        {
            OnEnd();
        }
        else if (action.GetName() == "UP")
        {
            input.up = true;
        }
        else if (action.GetName() == "LEFT")
        {
            input.left = true;
        }
        else if (action.GetName() == "DOWN")
        {
            input.down = true;
        }
        else if (action.GetName() == "RIGHT")
        {
            input.right = true;
        }
    }
    else if (action.GetType() == "END")
    {
        if (action.GetName() == "UP")
        {
            input.up = false;
        }
        else if (action.GetName() == "LEFT")
        {
            input.left = false;
        }
        else if (action.GetName() == "DOWN")
        {
            input.down = false;
        }
        else if (action.GetName() == "RIGHT")
        {
            input.right = false;
        }
    }
}

void ScenePlay::MoveEntities(float deltaTime)
{
    auto& input           = player->GetComponent<InputComponent>();
    auto& playerTransform = player->GetComponent<TransformComponent>();

    playerTransform.ResetVelocity();

    if (input.left)
    {
        playerTransform.velocity.x = -playerTransform.speed;
    }
    if (input.right)
    {
        playerTransform.velocity.x = playerTransform.speed;
    }
    if (input.up)
    {
        playerTransform.velocity.y = -playerTransform.speed;
    }
    if (input.down)
    {
        playerTransform.velocity.y = playerTransform.speed;
    }

    playerTransform.position += playerTransform.velocity * deltaTime;
    playerTransform.position.x =
        std::clamp(playerTransform.position.x, 0.0f, static_cast<float>(Game::WINDOW_WIDTH));
    playerTransform.position.y =
        std::clamp(playerTransform.position.y, 0.0f, static_cast<float>(Game::WINDOW_HEIGHT));
}

void ScenePlay::Render()
{
    auto& game         = Game::GetGame();
    auto& assetManager = game.GetAssetManager();

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);  // Set the clear color to grey
    glClear(GL_COLOR_BUFFER_BIT);          // Clear the color buffer

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // draw player
    auto& playerTransform = player->GetComponent<TransformComponent>();
    auto& playerSprite    = player->GetComponent<SpriteComponent>();

    auto& spriteShader = assetManager.GetShader(playerSprite.shaderName);
    auto& spriteVertex = assetManager.GetSpriteVertex();
    auto& texture      = assetManager.GetTexture(playerSprite.textureName);

    spriteShader.SetActive();
    spriteVertex.SetActive();

    spriteShader.SetVector2Uniform("uWindowSize", Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
    spriteShader.SetVector2Uniform("uTextureSize", texture.GetWidth(), texture.GetHeight());
    spriteShader.SetVector2Uniform("uTexturePosition", playerTransform.position);
    spriteShader.SetFloatUniform("uTextureScale", playerTransform.scale);

    texture.SetActive();
    glDrawElements(GL_TRIANGLES, spriteVertex.GetNumIndices(), GL_UNSIGNED_INT, nullptr);

    // draw text
    auto& fontTexture = assetManager.GetTexture("title");
    spriteShader.SetVector2Uniform("uTextureSize", fontTexture.GetWidth(), fontTexture.GetHeight());
    spriteShader.SetVector2Uniform("uTexturePosition",
                                   glm::vec2 {Game::WINDOW_WIDTH / 2.0f, fontTexture.GetHeight()});
    spriteShader.SetFloatUniform("uTextureScale", 3.0f);
    fontTexture.SetActive();
    glDrawElements(GL_TRIANGLES, spriteVertex.GetNumIndices(), GL_UNSIGNED_INT, nullptr);

    // swap the buffers
    SDL_GL_SwapWindow(game.GetWindow());
}
