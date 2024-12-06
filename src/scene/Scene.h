#pragma once

#include <map>
#include "Action.h"

using ActionMap = std::map<int, std::string>;

class Scene
{
public:
    Scene();
    Scene(const int sceneId);

    virtual void Update(float deltaTime)        = 0;
    virtual void DoAction(const Action& action) = 0;
    virtual void Render()                       = 0;

    void RegisterAction(int inputKey, const std::string& actionName);

    const ActionMap& GetActionMap() const;

protected:
    virtual void OnEnd() = 0;

    int id = 0;
    ActionMap actionMap;
};
