#pragma once

#include <vector>
#define GLM_FORCE_PURE
#include <glm/vec2.hpp>

class Texture;

class Animation
{
public:
    Animation();
    Animation(const std::vector<Texture>& t);
    Animation(const std::vector<Texture>& t, int speed);

    void Update();
    void SetActive();
    void Unload();

    Texture& GetTexture();

private:
    std::vector<Texture> textures;
    int currentFrame;
    int speed;
};
