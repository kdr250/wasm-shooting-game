#include "Font.h"
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

Texture* Font::RenderText(const std::string& text)
{
    // Convert to SDL_Color
    SDL_Color sdlColor;
    sdlColor.r = static_cast<Uint8>(255);
    sdlColor.g = static_cast<Uint8>(255);
    sdlColor.b = static_cast<Uint8>(255);
    sdlColor.a = 255;

    // Draw this to a surface (blended for alpha)
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), sdlColor);
    if (!surface)
    {
        return nullptr;
    }

    // Convert from surface to texture
    Texture* texture = new Texture(surface);

    SDL_FreeSurface(surface);

    return texture;
}
