#include "AudioManager.h"
#include <SDL2/SDL_log.h>

AudioManager::AudioManager() {}

bool AudioManager::LoadMusic(const std::string& name, const std::string& path)
{
    if (musics.contains(name))
    {
        return true;
    }

    auto music = Mix_LoadMUS(path.c_str());
    if (!music)
    {
        SDL_Log("Failed to load music: %s", Mix_GetError());
        return false;
    }

    musics.emplace(name, music);
    return true;
}

void AudioManager::PlayMusic(const std::string& name, bool loop)
{
    auto iter = musics.find(name);
    if (iter == musics.end())
    {
        SDL_Log("Failed to find music: %s", name.c_str());
        return;
    }

    int loopFlag = static_cast<int>(loop) * -1;
    auto music   = (*iter).second;

    if (Mix_PlayMusic(music, loopFlag) != 0)
    {
        SDL_Log("Failed to play music: %s", name.c_str());
        return;
    }
}

bool AudioManager::LoadSound(const std::string& name, const std::string& path)
{
    if (sounds.contains(name))
    {
        return true;
    }

    auto sound = Mix_LoadWAV(path.c_str());
    if (!sound)
    {
        SDL_Log("Failed to load sound: %s", Mix_GetError());
        return false;
    }

    sounds.emplace(name, sound);
    return true;
}

void AudioManager::PlaySound(const std::string& name, bool loop)
{
    auto iter = sounds.find(name);
    if (iter == sounds.end())
    {
        SDL_Log("Failed to find sound: %s", name.c_str());
        return;
    }

    int loopFlag = static_cast<int>(loop) * -1;
    auto sound   = (*iter).second;

    if (Mix_PlayChannel(-1, sound, loopFlag) != 0)
    {
        SDL_Log("Failed to play sound: %s", name.c_str());
        return;
    }
}

void AudioManager::Unload()
{
    for (auto& [name, music] : musics)
    {
        Mix_FreeMusic(music);
    }

    for (auto& [name, sound] : sounds)
    {
        Mix_FreeChunk(sound);
    }
}
