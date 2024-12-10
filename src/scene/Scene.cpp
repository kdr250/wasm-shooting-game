#include "Scene.h"
#include "../Game.h"

Scene::Scene() {}

Scene::Scene(const int sceneId) : id(sceneId)
{
    auto& game      = Game::GetGame();
    startTimeMillis = game.ElapsedTimeMillisecond();
}

void Scene::RegisterAction(int inputKey, const std::string& actionName)
{
    actionMap.emplace(inputKey, actionName);
}

const ActionMap& Scene::GetActionMap() const
{
    return actionMap;
}

long Scene::ElapsedTimeSecond()
{
    return ElapsedTimeMillisecond() / 1000;
}

long Scene::ElapsedTimeMillisecond()
{
    auto& game   = Game::GetGame();
    long elapsed = game.ElapsedTimeMillisecond();
    return elapsed - startTimeMillis;
}
