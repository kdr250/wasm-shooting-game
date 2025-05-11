#pragma once

#include <memory>
#include <string>
#define GLM_FORCE_PURE
#include <glm/vec2.hpp>

class Entity;

class PlayerLife
{
public:
    static void Spawn(const int max, const glm::vec2& position);
    static void Damaged(int damage);
    static void Draw();

    static int GetLife();

    static void Unload();

private:
    static std::shared_ptr<Entity>& GetPlayerLife();

    static std::string GenerateTextureName(const char numChar);
    static std::string GenerateTextureName(const int num);

    static int life;
    static int maxLife;

    inline static const std::string SPRITE_SHADER_VERT = "resources/shader/Sprite.vert";
    inline static const std::string SPRITE_SHADER_FRAG = "resources/shader/Sprite.frag";
    inline static const std::string SPRITE_SHADER_NAME = "sprite";

    inline static const std::string FONT_PATH = "resources/font/Roboto-Bold.ttf";
    inline static const std::string FONT_NAME = "Roboto-Bold";

    inline static const std::string PLAYER_LIFE_TEXTURE_PATH   = "resources/texture/example.png";
    inline static const std::string PLAYER_LIFE_TEXTURE_PREFIX = "text_number_";
    inline static const std::string PLAYER_LIFE_TEXTURE_NAME   = "PlayerLife";

    inline static const std::string PLAYER_LIFE_TAG = "PlayerLife";

    static constexpr char X = 'x';

    static constexpr int MAX_LIFE  = 99;
    static constexpr int MAX_DIGIT = 2;
};
