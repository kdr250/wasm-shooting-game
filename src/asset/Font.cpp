#include "Font.h"

#include <GL/glew.h>
#include <vector>
#include "Texture.h"

Font::Font() {}

Font::Font(const std::string& path)
{
    std::vector<int> fontSizes = {8,  9,  10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32,
                                  34, 36, 38, 40, 42, 44, 46, 48, 52, 56, 60, 64, 68, 72};

    for (auto& size : fontSizes)
    {
        TTF_Font* font = TTF_OpenFont(path.c_str(), size);
        if (!font)
        {
            SDL_Log("Failed to load Font %s , size %d", path.c_str(), size);
            exit(EXIT_FAILURE);
        }
        fonts.emplace(size, font);
    }
}

void Font::Unload()
{
    for (auto& font : fonts)
    {
        TTF_CloseFont(font.second);
    }
}

Texture Font::RenderText(const std::string& text, const SDL_Color& color, int size)
{
    // Find the font data for this point size
    auto iter = fonts.find(size);
    if (iter == fonts.end())
    {
        SDL_Log("Point size %d is unsupported, so use default %d size", size, DEFAULT_SIZE_30);
        iter = fonts.find(DEFAULT_SIZE_30);
    }

    TTF_Font* font = iter->second;

    SDL_Color backColor = color;
    backColor.r++;
    Uint32 colorBit      = (Uint32)((color.b << 16) + (color.g << 8) + (color.r << 0));
    Uint32 colorBitAlpha = (Uint32)(0xff000000 + (color.b << 16) + (color.g << 8) + (color.r << 0));
    Uint32 backColorBit  = (Uint32)((backColor.b << 16) + (backColor.g << 8) + (backColor.r << 0));

    // Draw this to a surface
    SDL_Surface* textImage8Bit = TTF_RenderUTF8_Shaded(font, text.c_str(), color, backColor);

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
    memset(texture->pixels, backColorBit, texture->w * texture->h * texture->format->BytesPerPixel);
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
        if (pixels[i] == colorBitAlpha)
        {
            continue;
        }
        if (pixels[i] == colorBit)
        {
            pixels[i] |= 0xff000000;
        }
        else
        {
            pixels[i] = 0;
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
