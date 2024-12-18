#include "Animation.h"

Animation::Animation() {}

Animation::Animation(const std::string& name, const std::vector<Texture>& t) : Animation(name, t, 1)
{
}

Animation::Animation(const std::string& name, const std::vector<Texture>& tex, int speed) :
    name(name), speed(speed)
{
    textures = tex;
}

void Animation::Update()
{
    currentFrame = (currentFrame + 1) % textures.size();
}

std::string& Animation::GetName()
{
    return name;
}

Texture& Animation::GetTexture()
{
    return textures[currentFrame];
}
