#pragma once

#include <memory>
#include <string>
#include <vector>

class Entity;

class BackgroundShader
{
public:
    static void Spawn(const std::string& vertPath, const std::string& fragPath);

    static void Draw();

private:
    static const std::vector<std::shared_ptr<Entity>>& GetBackgroundShaders();

    inline static const std::string BACKGROUND_SHADER_NAME = "background_shader";

    inline static const std::string BACKGROUND_SHADER_TAG = "background_shader";
};
