#include "Animation.h"
#include "Texture.h"

Animation::Animation() {}

Animation::Animation(const std::vector<Texture>& t) : Animation(t, 1) {}

Animation::Animation(const std::vector<Texture>& tex, int speed) : speed(speed)
{
    textures = tex;
}

void Animation::Update()
{
    ++currentFrame;
}

void Animation::SetActive()
{
    GetTexture().SetActive();
}

void Animation::Unload()
{
    for (auto& texture : textures)
    {
        texture.Unload();
    }
}

Texture& Animation::GetTexture()
{
    int animationFrame = (currentFrame / speed) % textures.size();
    return textures[animationFrame];
}
