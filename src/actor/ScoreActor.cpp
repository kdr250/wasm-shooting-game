#include "ScoreActor.h"
#include "../Game.h"

int ScoreActor::score          = 0;
int ScoreActor::scoreToDisplay = 0;

void ScoreActor::Spawn(const glm::vec2& position)
{
    score          = 0;
    scoreToDisplay = 0;

    auto& game          = Game::GetGame();
    auto& assetManager  = game.GetAssetManager();
    auto& entityManager = game.GetEntityManger();

    if (!assetManager.LoadShader(SPRITE_SHADER_NAME, SPRITE_SHADER_VERT, SPRITE_SHADER_FRAG))
    {
        SDL_Log("Failed to load shader");
        exit(EXIT_FAILURE);
    }

    assetManager.CreateSpriteVertex();

    if (!assetManager.LoadFont(FONT_NAME, FONT_PATH))
    {
        SDL_Log("Failed to load font");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i <= 9; ++i)
    {
        std::string text        = std::to_string(i);
        auto& font              = assetManager.GetFont(FONT_NAME);
        auto texture            = font.RenderText(text, Font::DEFAULT_COLOR_WHITE, 30);
        std::string textureName = SCORE_TEXTURE_PREFIX + text;
        assetManager.AddTexture(textureName, texture);
    }

    auto scoreActor = entityManager.AddEntity(SCORE_TAG);
    scoreActor->AddComponent<TransformComponent>(position);
}

void ScoreActor::AddScore(const int scoreToAdd)
{
    score += scoreToAdd;
}

void ScoreActor::Draw()
{
    DramRoll();

    auto& assetManager = Game::GetGame().GetAssetManager();
    auto& vertexArray  = assetManager.GetSpriteVertex();

    auto& scoreActor           = GetScoreActor();
    auto& transform            = scoreActor->GetComponent<TransformComponent>();
    glm::vec2 originalPosition = transform.position;

    std::string strScore = std::to_string(scoreToDisplay);

    for (int i = 0; i < strScore.size(); ++i)
    {
        std::string textureName = SCORE_TEXTURE_PREFIX + strScore[i];

        auto& spriteShader = assetManager.GetShader(SPRITE_SHADER_NAME);
        auto& texture      = assetManager.GetTexture(textureName);

        spriteShader.SetActive();
        spriteShader.SetVector2Uniform("uWindowSize", Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
        spriteShader.SetVector2Uniform("uTextureSize", texture.GetWidth(), texture.GetHeight());
        spriteShader.SetVector2Uniform("uTexturePosition", transform.position);
        spriteShader.SetFloatUniform("uTextureScale", 3.0f);
        texture.SetActive();

        glDrawElements(GL_TRIANGLES, vertexArray.GetNumIndices(), GL_UNSIGNED_INT, nullptr);

        transform.position.x += texture.GetWidth() / 1.5f;
    }

    transform.position = originalPosition;
}

void ScoreActor::Unload()
{
    auto& assetManager = Game::GetGame().GetAssetManager();
    for (int i = 0; i <= 9; ++i)
    {
        std::string text        = std::to_string(i);
        std::string textureName = SCORE_TEXTURE_PREFIX + text;
        assetManager.RemoveTexture(textureName);
    }
}

const std::shared_ptr<Entity>& ScoreActor::GetScoreActor()
{
    auto& entityManager = Game::GetGame().GetEntityManger();
    return entityManager.GetEntities(SCORE_TAG)[0];
}

void ScoreActor::DramRoll()
{
    scoreToDisplay = std::min(score, scoreToDisplay + DRAM_ROLL_SPEED);
}
