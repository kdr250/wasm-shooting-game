#pragma once

#include <SDL2/SDL.h>

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

    static void ProcessInput();

    static void UpdateGame();

    static void GenerateOutput();

    static void Loop();

    static bool IsRunning()
    {
        return game->isRunning;
    }

    Game();

    static Game* game;

    SDL_Window* window;

    SDL_Renderer* renderer;

    bool isRunning;
};
