#include "SceneMenu.h"
#include "../Game.h"
#include "ScenePlay.h"

// FIXME
std::string shaderName = "sprite";
std::string title      = "title";

SceneMenu::SceneMenu()
{
    auto& game         = Game::GetGame();
    auto& assetManager = game.GetAssetManager();

    if (!assetManager.LoadShader(shaderName, SPRITE_SHADER_VERT, SPRITE_SHADER_FRAG))
    {
        SDL_Log("Failed to load shader");
        exit(EXIT_FAILURE);
    }

    assetManager.CreateSpriteVertex();

    assetManager.LoadFont(FONT_NAME, FONT_PATH);
    auto& font       = assetManager.GetFont(FONT_NAME);
    auto fontTexture = font.RenderText("Press Enter To Start!", SDL_Color {0, 255, 0, 255}, 40);
    assetManager.AddTexture(title, fontTexture);

    RegisterAction(SDL_SCANCODE_RETURN, "PLAY_GAME");
    RegisterAction(SDL_SCANCODE_RETURN2, "PLAY_GAME");
    RegisterAction(SDL_SCANCODE_KP_ENTER, "PLAY_GAME");
}

void SceneMenu::Update(float deltaTime) {}

void SceneMenu::DoAction(const Action& action)
{
    if (action.type == "START")
    {
        if (action.name == "PLAY_GAME")
        {
            OnEnd();
        }
    }
}

void SceneMenu::Render()
{
    auto& game         = Game::GetGame();
    auto& assetManager = game.GetAssetManager();

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);  // Set the clear color to blue
    glClear(GL_COLOR_BUFFER_BIT);          // Clear the color buffer

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto& spriteShader = assetManager.GetShader(shaderName);
    auto& spriteVertex = assetManager.GetSpriteVertex();

    spriteShader.SetActive();
    spriteVertex.SetActive();

    // draw text
    auto& fontTexture = assetManager.GetTexture(title);
    spriteShader.SetVector2Uniform("uWindowSize", Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
    spriteShader.SetVector2Uniform("uTextureSize", fontTexture.GetWidth(), fontTexture.GetHeight());
    spriteShader.SetVector2Uniform(
        "uTexturePosition",
        glm::vec2 {Game::WINDOW_WIDTH / 2.0f, Game::WINDOW_HEIGHT / 2.0f});
    spriteShader.SetFloatUniform("uTextureScale", 3.0f);
    fontTexture.SetActive();
    glDrawElements(GL_TRIANGLES, spriteVertex.GetNumIndices(), GL_UNSIGNED_INT, nullptr);

    // swap the buffers
    SDL_GL_SwapWindow(game.GetWindow());
}

void SceneMenu::OnEnd()
{
    auto& game         = Game::GetGame();
    auto& assetManager = game.GetAssetManager();

    assetManager.RemoveTexture(title);
    game.ChangeScene("PLAY", std::make_shared<ScenePlay>(id + 1));
}
