#include "Animation.h"
#include "Texture.h"

Animation::Animation() {}

Animation::Animation(const std::vector<Texture>& t)
{
    textures = t;
}

void Animation::Unload()
{
    for (auto& texture : textures)
    {
        texture.Unload();
    }
}

Texture& Animation::GetTexture(const int currentFrame, const int speed)
{
    int animationFrame = (currentFrame / speed) % textures.size();
    return textures[animationFrame];
}
