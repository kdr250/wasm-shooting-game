#pragma once

#include "Scene.h"

class Entity;

class ScenePlay : public Scene
{
public:
    ScenePlay(const int sceneId, const int playerLife, const int score);

    void Update(float deltaTime) override;
    void DoAction(const Action& action) override;
    void Render() override;
    void OnClear() override;
    void OnOver() override;

    void SpawnEntities(float deltaTime);
    void MoveEntities(float deltaTime);
    void ProcessLifespan(float deltaTime);
    void ProcessCollision();

private:
    void OnEnd() override;

    void SetPause(bool pause);

    bool paused  = false;
    bool cleared = false;
    bool over    = false;

    bool hasNextScene = false;
};
