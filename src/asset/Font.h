#pragma once

#include <SDL2/SDL_ttf.h>
#include <string>

class Texture;

class Font
{
public:
    Font();
    Font(TTF_Font* f);
    Font(const std::string& path);

    void Unload();

    Texture RenderText(const std::string& text);  // FIXME: color, size

private:
    int NextPowerOfTwo(int value);

    TTF_Font* font;
};
