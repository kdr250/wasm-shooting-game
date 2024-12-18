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

    void Unload();

    Texture& GetTexture(const int currentFrame, const int speed);

private:
    std::vector<Texture> textures;
    glm::vec2 size;
};
