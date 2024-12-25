#pragma once

#include <SDL2/SDL_mixer.h>
#include <map>
#include <string>

class AudioManager
{
public:
    AudioManager();

    bool LoadMusic(const std::string& name, const std::string& path);
    void PlayMusic(const std::string& name, bool loop);

    void Unload();

private:
    std::map<std::string, Mix_Music*> sounds;
};
