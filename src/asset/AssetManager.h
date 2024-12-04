#pragma once

#include <map>
#include <string>
#include "Shader.h"
#include "Texture.h"

class AssetManager
{
public:
    AssetManager();

    bool LoadTexture(const std::string& name, const std::string& path);
    Texture& GetTexture(const std::string& name);

    bool LoadShader(const std::string& name,
                    const std::string& vertPath,
                    const std::string& fragPath);
    Shader& GetShader(const std::string& name);

private:
    bool ReadFile(const std::string& path, std::string& outContents);

    std::map<std::string, Texture> textures;
    std::map<std::string, Shader> shaders;
};
