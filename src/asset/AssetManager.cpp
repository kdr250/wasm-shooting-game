#include "AssetManager.h"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <fstream>
#include <sstream>

AssetManager::AssetManager() {}

bool AssetManager::LoadTexture(const std::string& name, const std::string& path)
{
    if (textures.contains(name))
    {
        return true;
    }

    SDL_Surface* surface = IMG_Load(path.c_str());

    if (!surface)
    {
        SDL_Log("Failed to load texture file: %s", path.c_str());
        return false;
    }

    int width  = surface->w;
    int height = surface->h;
    int format = SDL_ISPIXELFORMAT_ALPHA(surface->format->format) ? GL_RGBA : GL_RGB;

    Texture texture(format, width, height, surface->pixels);
    textures.emplace(name, texture);

    SDL_FreeSurface(surface);

    return true;
}

void AssetManager::AddTexture(const std::string& name, const Texture& texture)
{
    textures.emplace(name, texture);
}

Texture& AssetManager::GetTexture(const std::string& name)
{
    return textures[name];
}

bool AssetManager::LoadAnimation(const std::string& name,
                                 const std::string& path,
                                 const glm::vec2& oneFrameSize)
{
    if (animations.contains(name))
    {
        return true;
    }

    SDL_Surface* surface = IMG_Load(path.c_str());

    if (!surface)
    {
        SDL_Log("Failed to load texture file: %s", path.c_str());
        return false;
    }

    int width  = surface->w;
    int height = surface->h;
    int format = SDL_ISPIXELFORMAT_ALPHA(surface->format->format) ? GL_RGBA : GL_RGB;

    int oneFrameWidth  = static_cast<int>(oneFrameSize.x);
    int oneFrameHeight = static_cast<int>(oneFrameSize.y);
    int widthCount     = width / oneFrameWidth;
    int heightCount    = height / oneFrameHeight;

    std::vector<Texture> textures;

    SDL_Surface* split = SDL_CreateRGBSurfaceWithFormat(0,
                                                        oneFrameWidth,
                                                        oneFrameHeight,
                                                        32,
                                                        surface->format->format);

    SDL_Rect source {0, 0, oneFrameWidth, oneFrameHeight};
    SDL_Rect destination {0, 0, oneFrameWidth, oneFrameHeight};

    for (int i = 0; i < widthCount; ++i)
    {
        for (int j = 0; j < heightCount; ++j)
        {
            source.x = i * oneFrameWidth;
            source.y = j * oneFrameHeight;
            SDL_BlitSurface(surface, &source, split, &destination);
            Texture texture(format, oneFrameWidth, oneFrameHeight, split->pixels);
            textures.emplace_back(texture);
        }
    }

    SDL_FreeSurface(split);
    SDL_FreeSurface(surface);

    Animation animation(textures);
    animations.emplace(name, animation);

    return true;
}

Animation& AssetManager::GetAnimation(const std::string& name)
{
    return animations[name];
}

void AssetManager::RemoveAnimation(const std::string& name)
{
    auto iter = animations.find(name);
    if (iter != animations.end())
    {
        iter->second.Unload();
        animations.erase(iter);
    }
}

void AssetManager::RemoveTexture(const std::string& name)
{
    auto iter = textures.find(name);
    if (iter != textures.end())
    {
        iter->second.Unload();
        textures.erase(iter);
    }
}

bool AssetManager::LoadShader(const std::string& name,
                              const std::string& vertPath,
                              const std::string& fragPath)
{
    if (shaders.contains(name))
    {
        return true;
    }

    std::string vertContents;
    std::string fragContents;
    if (!ReadFile(vertPath, vertContents) || !ReadFile(fragPath, fragContents))
    {
        return false;
    }

    Shader shader(vertContents, fragContents);
    shaders.emplace(name, shader);

    return true;
}

Shader& AssetManager::GetShader(const std::string& name)
{
    return shaders[name];
}

bool AssetManager::LoadFont(const std::string& name, const std::string& path)
{
    if (fonts.contains(name))
    {
        return true;
    }

    Font font(path);
    fonts.emplace(name, font);
    return true;
}

Font& AssetManager::GetFont(const std::string& name)
{
    return fonts[name];
}

void AssetManager::CreateSpriteVertex()
{
    if (!spriteVertex)
    {
        spriteVertex = new VertexArray();
    }
}

bool AssetManager::ReadFile(const std::string& path, std::string& outContents)
{
    // Open file
    std::ifstream file(path);
    if (file.is_open())
    {
        // Read all the text into a string
        std::stringstream sstream;
        sstream << file.rdbuf();
        outContents = sstream.str();
        return true;
    }

    SDL_Log("file not found: %s", path.c_str());
    return false;
}
