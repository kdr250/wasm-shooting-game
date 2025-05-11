#include "PlayerLife.h"
#include "../Game.h"

int PlayerLife::life    = 0;
int PlayerLife::maxLife = 0;

void PlayerLife::Spawn(const int max, const glm::vec2& position)
{
    maxLife = max;
    life    = max;

    auto& game          = Game::GetGame();
    auto& assetManager  = game.GetAssetManager();
    auto& entityManager = game.GetEntityManger();

    if (!assetManager.LoadShader(SPRITE_SHADER_NAME, SPRITE_SHADER_VERT, SPRITE_SHADER_FRAG))
    {
        SDL_Log("Failed to load shader");
        exit(EXIT_FAILURE);
    }

    assetManager.CreateSpriteVertex();

    if (!assetManager.LoadTexture(PLAYER_LIFE_TEXTURE_NAME, PLAYER_LIFE_TEXTURE_PATH))
    {
        SDL_Log("Failed to load texture");
        exit(EXIT_FAILURE);
    }

    std::string text {X};
    auto& font              = assetManager.GetFont(FONT_NAME);
    auto texture            = font.RenderText(text, Font::DEFAULT_COLOR_WHITE, 30);
    std::string textureName = GenerateTextureName(X);
    assetManager.AddTexture(textureName, texture);

    for (int i = 0; i <= 9; ++i)
    {
        std::string text        = std::to_string(i);
        auto& font              = assetManager.GetFont(FONT_NAME);
        auto texture            = font.RenderText(text, Font::DEFAULT_COLOR_WHITE, 30);
        std::string textureName = GenerateTextureName(i);
        assetManager.AddTexture(textureName, texture);
    }

    auto playerLife = entityManager.AddEntity(PLAYER_LIFE_TAG);
    playerLife->AddComponent<TransformComponent>(position);
    playerLife->AddComponent<SpriteComponent>(SPRITE_SHADER_NAME, PLAYER_LIFE_TEXTURE_NAME);
}

void PlayerLife::Damaged(int damage)
{
    life -= damage;
}

void PlayerLife::Draw()
{
    auto& assetManager = Game::GetGame().GetAssetManager();

    auto& playerLife = GetPlayerLife();

    auto& transform = playerLife->GetComponent<TransformComponent>();
    auto& sprite    = playerLife->GetComponent<SpriteComponent>();

    glm::vec2 originalPosition = transform.position;

    // draw player texture
    auto& spriteShader  = assetManager.GetShader(sprite.shaderName);
    auto& playerTexture = assetManager.GetTexture(sprite.textureName);
    auto& vertexArray   = assetManager.GetSpriteVertex();

    spriteShader.SetActive();
    vertexArray.SetActive();

    spriteShader.SetVector2Uniform("uWindowSize", Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
    spriteShader.SetVector2Uniform("uTextureSize",
                                   playerTexture.GetWidth(),
                                   playerTexture.GetHeight());
    spriteShader.SetVector2Uniform("uTexturePosition", transform.position);
    spriteShader.SetFloatUniform("uTextureScale", 5.0f);

    playerTexture.SetActive();
    glDrawElements(GL_TRIANGLES, vertexArray.GetNumIndices(), GL_UNSIGNED_INT, nullptr);

    transform.position.x += 45.0f;
    transform.position.y -= 18.0f;

    // draw X
    auto& xTexture = assetManager.GetTexture(GenerateTextureName(X));

    spriteShader.SetActive();
    vertexArray.SetActive();
    spriteShader.SetVector2Uniform("uWindowSize", Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
    spriteShader.SetVector2Uniform("uTextureSize", xTexture.GetWidth(), xTexture.GetHeight());
    spriteShader.SetVector2Uniform("uTexturePosition", transform.position);
    spriteShader.SetFloatUniform("uTextureScale", 3.0f);
    xTexture.SetActive();
    glDrawElements(GL_TRIANGLES, vertexArray.GetNumIndices(), GL_UNSIGNED_INT, nullptr);

    transform.position.x += 30.0f;

    // draw number
    std::string strLife = std::to_string(life);
    for (int index = 0; index < MAX_DIGIT; ++index)
    {
        int i                   = strLife.size() - (MAX_DIGIT - index);
        char numChar            = i >= 0 ? strLife[i] : '0';
        std::string textureName = GenerateTextureName(numChar);

        auto& numTexture = assetManager.GetTexture(textureName);

        spriteShader.SetActive();
        vertexArray.SetActive();
        spriteShader.SetVector2Uniform("uWindowSize", Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
        spriteShader.SetVector2Uniform("uTextureSize",
                                       numTexture.GetWidth(),
                                       numTexture.GetHeight());
        spriteShader.SetVector2Uniform("uTexturePosition", transform.position);
        spriteShader.SetFloatUniform("uTextureScale", 3.0f);
        numTexture.SetActive();

        glDrawElements(GL_TRIANGLES, vertexArray.GetNumIndices(), GL_UNSIGNED_INT, nullptr);

        transform.position.x += numTexture.GetWidth() / 1.5f;
    }

    transform.position = originalPosition;
}

int PlayerLife::GetLife()
{
    return life;
}

void PlayerLife::Unload() {}

std::shared_ptr<Entity>& PlayerLife::GetPlayerLife()
{
    auto& entityManager = Game::GetGame().GetEntityManger();
    return entityManager.GetEntities(PLAYER_LIFE_TAG)[0];
}

std::string PlayerLife::GenerateTextureName(const char numChar)
{
    return PLAYER_LIFE_TEXTURE_PREFIX + numChar;
}

std::string PlayerLife::GenerateTextureName(const int num)
{
    return PLAYER_LIFE_TEXTURE_PREFIX + std::to_string(num);
}
