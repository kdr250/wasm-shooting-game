#pragma once

#include <SDL2/SDL.h>
#include <map>
#include <memory>
#include <string>
#include "asset/AssetManager.h"
#include "audio/AudioManager.h"
#include "ecs/EntityManager.h"
#include "scene/Action.h"

class Scene;

class Game
{
public:
    static constexpr int WINDOW_WIDTH  = 1024;
    static constexpr int WINDOW_HEIGHT = 768;

    inline static const std::string SPRITE_SHADER_VERT = "resources/shader/Sprite.vert";
    inline static const std::string SPRITE_SHADER_FRAG = "resources/shader/Sprite.frag";

    inline static const std::string PLAYER_TEXTURE = "resources/texture/example.png";
    inline static const std::string FONT_PATH      = "resources/font/Roboto-Bold.ttf";
    inline static const std::string FONT_NAME      = "Roboto-Bold";

    // Initailize game
    static bool Initialize();

    // Runs the game loop until the game is over
    static void RunLoop();

    static Game& GetGame();

    AssetManager& GetAssetManager();

    EntityManager& GetEntityManger();

    AudioManager& GetAudioManager();

    SDL_Window* GetWindow();

    float GetDeltaTime();
    long ElapsedTimeSecond();
    long ElapsedTimeMillisecond();
    long SceneElapsedTimeSecond();
    long SceneElapsedTimeMillisecond();

    void Stop();

    void ChangeScene(const std::string& sceneName,
                     std::shared_ptr<Scene> scene,
                     bool endCurrentScene = false);

    void OnSceneClear();
    void OnSceneOver();

private:
    // Shutdown the game
    static void Shutdown();

    static void Loop();

    void ProcessInput();

    void UpdateGame();

    void GenerateOutput();

    Game();

    static Game* game;

    SDL_Window* window;

    SDL_GLContext context;

    EntityManager entityManager;
    AssetManager assetManager;
    AudioManager soundManager;

    bool isRunning;

    Uint64 tickCount;
    float deltaTime;

    Action action;

    std::map<std::string, std::shared_ptr<Scene>> sceneMap;
    std::string currentSceneName;
};
