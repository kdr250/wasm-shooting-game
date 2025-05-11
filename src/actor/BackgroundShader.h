#pragma once

#include <memory>
#include <string>
#include <vector>

class Entity;

class BackgroundShader
{
public:
    static void Spawn();

    static void Draw();

private:
    static const std::shared_ptr<Entity>& GetBackgroundShader();

    inline static const std::string BACKGROUND_SHADER_VERT = "resources/shader/Background.vert";
    inline static const std::string BACKGROUND_SHADER_FRAG = "resources/shader/Background.frag";
    inline static const std::string BACKGROUND_SHADER_NAME = "background";

    inline static const std::string BACKGROUND_SHADER_TAG = "background_shader";
};
