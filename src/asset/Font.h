#pragma once

#include <SDL2/SDL_ttf.h>
#include <string>

class Font
{
public:
    Font();
    Font(TTF_Font* f);
    Font(const std::string& path);

    void Unload();

    class Texture* RenderText(const std::string& text);  // FIXME: color, size

private:
    TTF_Font* font;
};
