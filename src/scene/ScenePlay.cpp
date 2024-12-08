#include "ScenePlay.h"
#include <SDL2/SDL_scancode.h>
#include "../Game.h"
#include "Action.h"

ScenePlay::ScenePlay(const int sceneId) : Scene(sceneId)
{
    auto& game          = Game::GetGame();
    auto& assetManager  = game.GetAssetManager();
    auto& entityManager = game.GetEntityManger();

    std::string PLAYER_TEXTURE_NAME = "example";

    if (!assetManager.LoadShader(SPRITE_SHADER_NAME, SPRITE_SHADER_VERT, SPRITE_SHADER_FRAG)
        || !assetManager.LoadShader(BULLET_SHADER_NAME, BULLET_SHADER_VERT, BULLET_SHADER_FRAG))
    {
        SDL_Log("Failed to load shader");
        exit(EXIT_FAILURE);
    }

    assetManager.CreateSpriteVertex();

    if (!assetManager.LoadTexture(PLAYER_TEXTURE_NAME, PLAYER_TEXTURE))
    {
        SDL_Log("Failed to load texture");
        exit(EXIT_FAILURE);
    }

    assetManager.LoadFont(FONT_NAME, FONT_PATH);
    auto& font       = assetManager.GetFont(FONT_NAME);
    auto fontTexture = font.RenderText("Hello World !!", Font::DEFAULT_COLOR_WHITE, 40);
    assetManager.AddTexture("title", fontTexture);

    player = entityManager.AddEntity("player");
    player->AddComponent<StateComponent>();
    player->AddComponent<TransformComponent>(
        glm::vec2 {Game::WINDOW_WIDTH / 2.0, Game::WINDOW_HEIGHT / 2.0},  // position
        10.0f,                                                            // scale
        200.0f                                                            // speed
    );
    player->AddComponent<InputComponent>();
    player->AddComponent<SpriteComponent>(SPRITE_SHADER_NAME, PLAYER_TEXTURE_NAME);

    RegisterAction(SDL_SCANCODE_W, "UP");
    RegisterAction(SDL_SCANCODE_A, "LEFT");
    RegisterAction(SDL_SCANCODE_S, "DOWN");
    RegisterAction(SDL_SCANCODE_D, "RIGHT");
    RegisterAction(SDL_SCANCODE_SPACE, "SHOOT");
    RegisterAction(SDL_SCANCODE_P, "PAUSE");
    RegisterAction(SDL_SCANCODE_ESCAPE, "QUIT");
}

void ScenePlay::Update(float deltaTime)
{
    Game::GetGame().GetEntityManger().Update();

    MoveEntities(deltaTime);
}

void ScenePlay::OnEnd()
{
    Game::GetGame().Stop();
}

void ScenePlay::DoAction(const Action& action)
{
    auto& input = player->GetComponent<InputComponent>();

    if (action.type == "START")
    {
        if (action.name == "QUIT")
        {
            OnEnd();
        }
        else if (action.name == "UP")
        {
            input.up = true;
        }
        else if (action.name == "LEFT")
        {
            input.left = true;
        }
        else if (action.name == "DOWN")
        {
            input.down = true;
        }
        else if (action.name == "RIGHT")
        {
            input.right = true;
        }
        else if (action.name == "SHOOT")
        {
            input.shoot = true;
        }
    }
    else if (action.type == "END")
    {
        if (action.name == "UP")
        {
            input.up = false;
        }
        else if (action.name == "LEFT")
        {
            input.left = false;
        }
        else if (action.name == "DOWN")
        {
            input.down = false;
        }
        else if (action.name == "RIGHT")
        {
            input.right = false;
        }
        else if (action.name == "SHOOT")
        {
            input.shoot = false;
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

    auto& assetManager = Game::GetGame().GetAssetManager();
    auto& playerSprite = player->GetComponent<SpriteComponent>();
    auto& texture      = assetManager.GetTexture(playerSprite.shaderName);
    float scale        = player->GetComponent<TransformComponent>().scale;

    playerTransform.position += playerTransform.velocity * deltaTime;
    playerTransform.position.x = std::clamp(playerTransform.position.x,
                                            texture.GetWidth() * scale / 4.0f,
                                            Game::WINDOW_WIDTH - texture.GetWidth() * scale / 4.0f);
    playerTransform.position.y =
        std::clamp(playerTransform.position.y,
                   texture.GetHeight() * scale / 4.0f,
                   Game::WINDOW_HEIGHT - texture.GetHeight() * scale / 4.0f);
}

void ScenePlay::Render()
{
    auto& game         = Game::GetGame();
    auto& assetManager = game.GetAssetManager();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Set the clear color to black
    glClear(GL_COLOR_BUFFER_BIT);          // Clear the color buffer

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto& vertexArray = assetManager.GetSpriteVertex();

    // Draw Bullet
    auto& bulletShader = assetManager.GetShader(BULLET_SHADER_NAME);
    bulletShader.SetActive();
    vertexArray.SetActive();
    bulletShader.SetVector2Uniform("uWindowSize", Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
    bulletShader.SetVector2Uniform("uBulletPosition", 200.0f, 300.0f);
    bulletShader.SetVector2Uniform("uBulletSize", 200.0f, 200.0f);
    glDrawElements(GL_TRIANGLES, vertexArray.GetNumIndices(), GL_UNSIGNED_INT, nullptr);

    // draw player
    auto& playerTransform = player->GetComponent<TransformComponent>();
    auto& playerSprite    = player->GetComponent<SpriteComponent>();

    auto& spriteShader = assetManager.GetShader(playerSprite.shaderName);
    auto& texture      = assetManager.GetTexture(playerSprite.textureName);

    spriteShader.SetActive();
    vertexArray.SetActive();

    spriteShader.SetVector2Uniform("uWindowSize", Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
    spriteShader.SetVector2Uniform("uTextureSize", texture.GetWidth(), texture.GetHeight());
    spriteShader.SetVector2Uniform("uTexturePosition", playerTransform.position);
    spriteShader.SetFloatUniform("uTextureScale", playerTransform.scale);

    texture.SetActive();
    glDrawElements(GL_TRIANGLES, vertexArray.GetNumIndices(), GL_UNSIGNED_INT, nullptr);

    // draw text
    auto& fontTexture = assetManager.GetTexture("title");
    spriteShader.SetVector2Uniform("uTextureSize", fontTexture.GetWidth(), fontTexture.GetHeight());
    spriteShader.SetVector2Uniform("uTexturePosition",
                                   glm::vec2 {Game::WINDOW_WIDTH / 2.0f, fontTexture.GetHeight()});
    spriteShader.SetFloatUniform("uTextureScale", 3.0f);
    fontTexture.SetActive();
    glDrawElements(GL_TRIANGLES, vertexArray.GetNumIndices(), GL_UNSIGNED_INT, nullptr);

    // swap the buffers
    SDL_GL_SwapWindow(game.GetWindow());
}
