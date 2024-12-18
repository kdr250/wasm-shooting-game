#include "AssetManager.h"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <png.h>
#include <fstream>
#include <sstream>

AssetManager::AssetManager() {}

bool AssetManager::LoadTexture(const std::string& name, const std::string& path)
{
    if (textures.contains(name))
    {
        return true;
    }

    FILE* pngFile = fopen(path.c_str(), "rb");
    if (!pngFile)
    {
        SDL_Log("Failed to open image file %s", path.c_str());
        return false;
    }

    auto pngStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!pngStruct)
    {
        SDL_Log("Failed to create PNG structure %s", path.c_str());
        return false;
    }

    auto pngInfo = png_create_info_struct(pngStruct);
    if (!pngInfo)
    {
        SDL_Log("Failed to create PNG info structure %s", path.c_str());
        png_destroy_read_struct(&pngStruct, nullptr, nullptr);
        pngStruct = nullptr;
        return false;
    }

    if (setjmp(png_jmpbuf(pngStruct)))
    {
        return false;
    }

    png_init_io(pngStruct, pngFile);

    png_read_info(pngStruct, pngInfo);

    auto width      = png_get_image_width(pngStruct, pngInfo);
    auto height     = png_get_image_height(pngStruct, pngInfo);
    auto rowLen     = png_get_rowbytes(pngStruct, pngInfo);
    auto colorType  = png_get_color_type(pngStruct, pngInfo);
    auto numChannel = png_get_channels(pngStruct, pngInfo);

    // prepare storage
    auto pixels  = std::make_unique<png_byte[]>(height * rowLen);
    auto rowPtrs = std::make_unique<png_bytep[]>(height);
    for (png_uint_32 i = 0; i < height; ++i)
    {
        rowPtrs[i] = &pixels[i * rowLen];
    }

    // read pixels
    png_read_image(pngStruct, rowPtrs.get());

    auto image = std::make_unique<unsigned char[]>(numChannel * width * height);
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < numChannel * width; ++j)
        {
            image[i * numChannel * width + j] = rowPtrs[i][j];
        }
    }

    png_destroy_read_struct(&pngStruct, &pngInfo, nullptr);
    fclose(pngFile);

    int format = colorType == PNG_COLOR_TYPE_RGB_ALPHA ? GL_RGBA : GL_RGB;

    Texture texture(format, width, height, image.get());
    textures.emplace(name, texture);

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
                                 const glm::vec2& oneFrameSize,
                                 const int speed)
{
    if (animations.contains(name))
    {
        return true;
    }

    FILE* pngFile = fopen(path.c_str(), "rb");
    if (!pngFile)
    {
        SDL_Log("Failed to open image file %s", path.c_str());
        return false;
    }

    auto pngStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!pngStruct)
    {
        SDL_Log("Failed to create PNG structure %s", path.c_str());
        return false;
    }

    auto pngInfo = png_create_info_struct(pngStruct);
    if (!pngInfo)
    {
        SDL_Log("Failed to create PNG info structure %s", path.c_str());
        png_destroy_read_struct(&pngStruct, nullptr, nullptr);
        pngStruct = nullptr;
        return false;
    }

    if (setjmp(png_jmpbuf(pngStruct)))
    {
        return false;
    }

    png_init_io(pngStruct, pngFile);

    png_read_info(pngStruct, pngInfo);

    auto width      = png_get_image_width(pngStruct, pngInfo);
    auto height     = png_get_image_height(pngStruct, pngInfo);
    auto rowLen     = png_get_rowbytes(pngStruct, pngInfo);
    auto colorType  = png_get_color_type(pngStruct, pngInfo);
    auto numChannel = png_get_channels(pngStruct, pngInfo);

    // prepare storage
    auto pixels  = std::make_unique<png_byte[]>(height * rowLen);
    auto rowPtrs = std::make_unique<png_bytep[]>(height);
    for (png_uint_32 i = 0; i < height; ++i)
    {
        rowPtrs[i] = &pixels[i * rowLen];
    }

    // read pixels
    png_read_image(pngStruct, rowPtrs.get());

    int oneFrameWidth  = static_cast<int>(oneFrameSize.x);
    int oneFrameHeight = static_cast<int>(oneFrameSize.y);
    int widthCount     = width / oneFrameWidth;
    int heightCount    = height / oneFrameHeight;

    std::vector<Texture> textures;

    for (int i = 0; i < heightCount; ++i)
    {
        for (int j = 0; j < widthCount; ++j)
        {
            auto image =
                std::make_unique<unsigned char[]>(numChannel * oneFrameWidth * oneFrameHeight);
            int index = 0;
            for (int k = i * oneFrameHeight; k < (i + 1) * oneFrameHeight; ++k)
            {
                for (int l = j * numChannel * oneFrameWidth;
                     l < (j + 1) * numChannel * oneFrameWidth;
                     ++l)
                {
                    image[index] = rowPtrs[k][l];
                    ++index;
                }
            }

            int format = colorType == PNG_COLOR_TYPE_RGB_ALPHA ? GL_RGBA : GL_RGB;

            Texture texture(format, oneFrameWidth, oneFrameHeight, image.get());
            textures.emplace_back(texture);
        }
    }

    png_destroy_read_struct(&pngStruct, &pngInfo, nullptr);
    fclose(pngFile);

    Animation animation(textures, speed);
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
