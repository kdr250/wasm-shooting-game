#include "SoundManager.h"
#include <SDL2/SDL_log.h>

SoundManager::SoundManager() {}

bool SoundManager::LoadSound(const std::string& name, const std::string& path)
{
    if (sounds.contains(name))
    {
        return true;
    }

    auto sound = Mix_LoadMUS(path.c_str());
    if (!sound)
    {
        SDL_Log("Failed to load music: %s", Mix_GetError());
        return false;
    }

    sounds.emplace(name, sound);
    return true;
}

void SoundManager::PlaySound(const std::string& name, bool loop)
{
    auto iter = sounds.find(name);
    if (iter == sounds.end())
    {
        SDL_Log("Failed to play music: %s", name.c_str());
        return;
    }

    int loopFlag      = static_cast<int>(loop) * -1;
    auto nameAndSound = *iter;

    Mix_PlayMusic(nameAndSound.second, loopFlag);
}

void SoundManager::Unload()
{
    for (auto& [name, sound] : sounds)
    {
        Mix_FreeMusic(sound);
    }
}
