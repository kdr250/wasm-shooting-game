#pragma once

#include <memory>

class Texture
{
public:
    Texture(int format, int width, int height, const void *pixels);

    void SetActive();

    void Unload();

    int GetWidth() const
    {
        return width;
    }

    int GetHeight() const
    {
        return height;
    }

private:
    // OpenGL ID of this texture
    unsigned int id;

    // Width/height of the texture
    int width;
    int height;
};
