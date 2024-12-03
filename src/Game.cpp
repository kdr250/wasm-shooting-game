#include "Game.h"
#include <algorithm>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

Game* Game::game = nullptr;

Game::Game() : window(nullptr), renderer(nullptr), isRunning(true), tickCount(0) {}

bool Game::Initialize()
{
    if (game)
    {
        SDL_Log("Game is already constructed!");
        exit(1);
    }

    game = new Game();

    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if (sdlResult != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    game->window = SDL_CreateWindow("Hello world !!",         // window title
                                    SDL_WINDOWPOS_UNDEFINED,  // Top left x-coordinate of window
                                    SDL_WINDOWPOS_UNDEFINED,  // Top left y-coordinate of window
                                    Game::WINDOW_WIDTH,       // width of window
                                    Game::WINDOW_HEIGHT,      // height of window
                                    0                         // flags (0 for no flags set)
    );

    if (!game->window)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    game->renderer = SDL_CreateRenderer(game->window,  // window to create renderer for
                                        -1,            // usually -1
                                        0);

    if (!game->renderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    game->playerPosition = glm::vec2 {Game::WINDOW_WIDTH / 2.0f - game->playerEdge,
                                      Game::WINDOW_HEIGHT / 2.0f - game->playerEdge};

    return true;
}

void Game::RunLoop()
{
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(Loop, 60, 1);
#else
    while (game->isRunning)
    {
        Loop();
    }
    Shutdown();
#endif
}

void Game::Loop()
{
#ifdef __EMSCRIPTEN__
    if (!game->isRunning)
    {
        emscripten_cancel_main_loop(); /* this should "kill" the app. */
        Shutdown();
        return;
    }
#endif
    game->ProcessInput();
    game->UpdateGame();
    game->GenerateOutput();
}

void Game::Shutdown()
{
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    SDL_Quit();
    delete game;
    game = nullptr;
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                isRunning = false;
                break;

            default:
                break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_ESCAPE])
    {
        isRunning = false;
    }

    playerVelocity.x = 0.0f;
    playerVelocity.y = 0.0f;

    if (state[SDL_SCANCODE_A])
    {
        playerVelocity.x = -playerSpeed;
    }
    if (state[SDL_SCANCODE_D])
    {
        playerVelocity.x = playerSpeed;
    }
    if (state[SDL_SCANCODE_W])
    {
        playerVelocity.y = -playerSpeed;
    }
    if (state[SDL_SCANCODE_S])
    {
        playerVelocity.y = playerSpeed;
    }
}

void Game::UpdateGame()
{
#ifndef __EMSCRIPTEN__
    // Wait until 16ms has elapsed since last frame
    while (!SDL_TICKS_PASSED(SDL_GetTicks64(), tickCount + 16))
        ;
#endif

    float deltaTime = (SDL_GetTicks64() - tickCount) / 1000.0f;
    deltaTime       = std::min(deltaTime, 0.05f);

    tickCount = SDL_GetTicks64();

    playerPosition += playerVelocity * deltaTime;
    playerPosition.x = std::clamp(playerPosition.x, 0.0f, Game::WINDOW_WIDTH - playerEdge);
    playerPosition.y = std::clamp(playerPosition.y, 0.0f, Game::WINDOW_HEIGHT - playerEdge);
}

void Game::GenerateOutput()
{
    // set draw color blue
    SDL_SetRenderDrawColor(renderer,
                           0,    // R
                           0,    // G
                           255,  // B
                           255   // A
    );

    // clear back buffer
    SDL_RenderClear(renderer);

    // draw player rectangle
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect player {
        static_cast<int>(playerPosition.x),  // top left x
        static_cast<int>(playerPosition.y),  // top left y
        static_cast<int>(playerEdge),        // width
        static_cast<int>(playerEdge)         // height
    };
    SDL_RenderFillRect(renderer, &player);

    // exchange front buffer for back buffer
    SDL_RenderPresent(renderer);
}
