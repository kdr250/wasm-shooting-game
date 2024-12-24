#pragma once

#include <SDL2/SDL_mixer.h>
#include <map>
#include <string>

class SoundManager
{
public:
    SoundManager();

    bool LoadSound(const std::string& name, const std::string& path);
    void PlaySound(const std::string& name, bool loop);

    void Unload();

private:
    std::map<std::string, Mix_Music*> sounds;
};
