#include "Font.h"

#include <GL/glew.h>
#include "Texture.h"

Font::Font() {}

Font::Font(TTF_Font* f) : font(f) {}

Font::Font(const std::string& path)
{
    font = TTF_OpenFont(path.c_str(), 24);
}

void Font::Unload()
{
    TTF_CloseFont(font);
}

Texture Font::RenderText(const std::string& text)
{
    // Convert to SDL_Color
    SDL_Color sdlColor;
    sdlColor.r = static_cast<Uint8>(255);
    sdlColor.g = static_cast<Uint8>(255);
    sdlColor.b = static_cast<Uint8>(255);
    sdlColor.a = 255;

    // Draw this to a surface
    SDL_Surface* textImage8Bit = TTF_RenderText_Solid(font, text.c_str(), sdlColor);

    // Convert surface from 8 to 32 bit for GL
    SDL_Surface* textImage = SDL_ConvertSurfaceFormat(textImage8Bit, SDL_PIXELFORMAT_RGBA8888, 0);

    // Create power of 2 dimensioned texture for GL with 1 texel border, clear it, and copy text image into it
    SDL_Surface* texture = SDL_CreateRGBSurface(0,
                                                NextPowerOfTwo(textImage->w + 2),
                                                NextPowerOfTwo(textImage->h + 2),
                                                textImage->format->BitsPerPixel,
                                                0,
                                                0,
                                                0,
                                                0);
    memset(texture->pixels, 0x0, texture->w * texture->h * texture->format->BytesPerPixel);
    SDL_Rect destRect = {(texture->w - textImage->w) / 2,
                         texture->h - textImage->h - 1,
                         textImage->w + 1,
                         texture->h - 1};
    SDL_SetSurfaceBlendMode(textImage, SDL_BLENDMODE_NONE);
    SDL_BlitSurface(textImage, NULL, texture, &destRect);

    // Emscripten/SDL bug? SDL_BlitSurface should copy source alpha when source surface is set to
    // SDL_BLENDMODE_NONE, however this is not happening, so fix it up here
    unsigned int* pixels = (unsigned int*)texture->pixels;
    for (int i = 0; i < texture->w * texture->h; ++i)
    {
        if (pixels[i] != 0)
        {
            pixels[i] |= 0xff000000;
        }
    }

    // Determine GL texture format
    GLint format = -1;
    if (texture->format->BitsPerPixel == 24)
        format = GL_RGB;
    else if (texture->format->BitsPerPixel == 32)
        format = GL_RGBA;

    Texture result(format, texture->w, texture->h, texture->pixels);
    SDL_FreeSurface(textImage8Bit);
    SDL_FreeSurface(textImage);
    SDL_FreeSurface(texture);

    return result;
}

int Font::NextPowerOfTwo(int value)
{
    int power = 1;
    while (power < value)
        power *= 2;
    return power;
}
