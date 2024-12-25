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

    bool LoadSound(const std::string& name, const std::string& path);
    void PlaySound(const std::string& name);

    void Unload();

private:
    std::map<std::string, Mix_Music*> musics;
    std::map<std::string, Mix_Chunk*> sounds;
    std::map<std::string, int> soundChannels;
};
