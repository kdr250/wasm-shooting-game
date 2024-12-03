#pragma once

#include <SDL2/SDL.h>
#define GLM_FORCE_PURE
#include <glm/vec2.hpp>

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

    bool isRunning;

    Uint64 tickCount;

    glm::vec2 playerPosition;
    glm::vec2 playerVelocity;
    float playerEdge  = 100.0f;
    float playerSpeed = 200.0f;
};
