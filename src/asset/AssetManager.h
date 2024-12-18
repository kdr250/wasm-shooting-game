#pragma once

#include <map>
#include <string>
#include "Animation.h"
#include "Font.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"

class AssetManager
{
public:
    AssetManager();

    bool LoadTexture(const std::string& name, const std::string& path);
    void AddTexture(const std::string& name, const Texture& texture);
    void RemoveTexture(const std::string& name);
    Texture& GetTexture(const std::string& name);

    bool LoadAnimation(const std::string& name,
                       const std::string& path,
                       const glm::vec2& oneFrameSize,
                       const int speed);
    Animation& GetAnimation(const std::string& name);

    bool LoadShader(const std::string& name,
                    const std::string& vertPath,
                    const std::string& fragPath);
    Shader& GetShader(const std::string& name);

    bool LoadFont(const std::string& name, const std::string& path);
    Font& GetFont(const std::string& name);

    void CreateSpriteVertex();
    VertexArray& GetSpriteVertex()
    {
        return *spriteVertex;
    }

private:
    bool ReadFile(const std::string& path, std::string& outContents);

    std::map<std::string, Texture> textures;
    std::map<std::string, Animation> animations;
    std::map<std::string, Shader> shaders;
    std::map<std::string, Font> fonts;

    VertexArray* spriteVertex = nullptr;
};
