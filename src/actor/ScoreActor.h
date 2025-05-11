#pragma once

#include <memory>
#include <string>
#define GLM_FORCE_PURE
#include <glm/vec2.hpp>

class Entity;

class ScoreActor
{
public:
    static void Spawn(const glm::vec2& position, const int inScore);
    static void AddScore(const int scoreToAdd);
    static int GetScore();
    static void Draw();

    static void Unload();

private:
    static const std::shared_ptr<Entity>& GetScoreActor();

    static void DramRoll();

    static std::string GenerateTextureName(const char numChar);
    static std::string GenerateTextureName(const int num);

    static int score;
    static int scoreToDisplay;

    inline static const std::string SPRITE_SHADER_VERT = "resources/shader/Sprite.vert";
    inline static const std::string SPRITE_SHADER_FRAG = "resources/shader/Sprite.frag";
    inline static const std::string SPRITE_SHADER_NAME = "sprite";

    inline static const std::string FONT_PATH = "resources/font/Roboto-Bold.ttf";
    inline static const std::string FONT_NAME = "Roboto-Bold";

    inline static const std::string SCORE_TAG            = "Score";
    inline static const std::string SCORE_TEXTURE_PREFIX = "text_number_";

    static constexpr int DRAM_ROLL_SPEED = 9;

    static constexpr int MAX_SCORE = 99999999;
    static constexpr int MAX_DIGIT = 8;
};
