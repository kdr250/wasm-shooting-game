#pragma once

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <unordered_map>

class Texture;

class Font
{
public:
    static constexpr int DEFAULT_SIZE_30 = 30;
    static constexpr SDL_Color DEFAULT_COLOR_WHITE {255, 255, 255, 255};

    Font();
    Font(const std::string& path);

    void Unload();

    // Given string and this font, draw to a texture
    Texture RenderText(const std::string& text,
                       const SDL_Color& color = DEFAULT_COLOR_WHITE,
                       int size               = DEFAULT_SIZE_30);

private:
    int NextPowerOfTwo(int value);

    // Map of point sizes to font data
    std::unordered_map<int, TTF_Font*> fonts;
};
