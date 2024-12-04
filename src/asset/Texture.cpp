#include "Texture.h"
#include <GL/glew.h>

Texture::Texture(int format, int width, int height, const void *pixels) :
    width(width), height(height)
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);

    // Enable bilinear filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::SetActive()
{
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::Unload()
{
    glDeleteTextures(1, &id);
}
