#pragma once

#include <SDL2/SDL.h>
#include "ecs/EntityManager.h"

class Game
{
public:
    static constexpr int WINDOW_WIDTH  = 1024;
    static constexpr int WINDOW_HEIGHT = 768;

    // Initailize game
    static bool Initialize();

    // Runs the game loop until the game is over
    static void RunLoop();

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

    SDL_Renderer* renderer;

    EntityManager entityManager;

    bool isRunning;

    Uint64 tickCount;

    std::shared_ptr<Entity> player;
};
