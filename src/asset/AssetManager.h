#pragma once

#include <map>
#include <string>
#include "Texture.h"

class AssetManager
{
public:
    AssetManager();

    bool LoadTexture(const std::string& name, const std::string& path);

    Texture& GetTexture(const std::string& name);

private:
    std::map<std::string, Texture> textures;
};
