#include "Scene.h"

Scene::Scene() {}

void Scene::RegisterAction(int inputKey, const std::string& actionName)
{
    actionMap.emplace(inputKey, actionName);
}

const ActionMap& Scene::GetActionMap() const
{
    return actionMap;
}
