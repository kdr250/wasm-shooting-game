#pragma once

#include <string>

class Background
{
public:
    static void Spawn();

    static void Draw();

private:
#ifdef __EMSCRIPTEN__
    inline static const std::string BACKGROUND_SHADER_VERT = "resources/shader/Background.vert";
    inline static const std::string BACKGROUND_SHADER_FRAG = "resources/shader/Background.frag";
#else
    inline static const std::string BACKGROUND_SHADER_VERT = "resources/shader/BackgroundV3.vert";
    inline static const std::string BACKGROUND_SHADER_FRAG = "resources/shader/BackgroundV3.frag";
#endif
    inline static const std::string BACKGROUND_SHADER_NAME = "background";

    inline static const std::string BACKGROUND_PATH_ONE = "resources/texture/background1.png";
    inline static const std::string BACKGROUND_NAME_ONE = "background1";

    inline static const std::string BACKGROUND_TAG = "background";
};
