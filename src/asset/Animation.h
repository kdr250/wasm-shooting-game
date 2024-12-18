#pragma once

#include <string>
#include <vector>
#define GLM_FORCE_PURE
#include <glm/vec2.hpp>

class Texture;

class Animation
{
public:
    Animation();
    Animation(const std::string& name, const std::vector<Texture>& t);
    Animation(const std::string& name, const std::vector<Texture>& t, int speed);

    void Update();

    std::string& GetName();
    Texture& GetTexture();

private:
    std::vector<Texture> textures;
    int currentFrame;
    int speed;
    std::string name;
};
