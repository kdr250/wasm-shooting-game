#include "Spawner.h"

#include <SDL2/SDL_log.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "BackgroundSpawner.h"
#include "EnemySpawner.h"

bool Spawner::Initialize(const int sceneId)
{
    std::string filePath = "resources/scene/" + std::to_string(sceneId) + ".txt";
    std::ifstream file;
    file.open(filePath);

    if (!file.is_open())
    {
        SDL_Log("Failed to open scene file: %s", filePath.c_str());
        exit(EXIT_FAILURE);
    }

    std::string line;
    std::vector<std::string> allLine;

    while (std::getline(file, line))
    {
        allLine.push_back(line);
    }

    file.close();

    EnemySpawner::Initialize(allLine);
    BackgroundSpawner::Spawn(allLine);

    std::string nextFilePath = "resources/scene/" + std::to_string(sceneId + 1) + ".txt";
    file.open(nextFilePath);
    bool hasNextScene = file.is_open();

    file.close();

    return hasNextScene;
}
