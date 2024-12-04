#include "AssetManager.h"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <png.h>

AssetManager::AssetManager() {}

bool AssetManager::LoadTexture(const std::string& name, const std::string& path)
{
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

Texture& AssetManager::GetTexture(const std::string& name)
{
    return textures[name];
}
