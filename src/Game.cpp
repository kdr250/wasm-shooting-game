#include "Game.h"

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

Game* Game::game = nullptr;

Game::Game() : window(nullptr), renderer(nullptr), isRunning(true) {}

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

    return true;
}

void Game::RunLoop()
{
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(Loop, 0, 1);
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
    if (!Game::IsRunning())
    {
        emscripten_cancel_main_loop(); /* this should "kill" the app. */
        Game::Shutdown();
        return;
    }
#endif
    ProcessInput();
    UpdateGame();
    GenerateOutput();
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
                game->isRunning = false;
                break;

            default:
                break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_ESCAPE])
    {
        game->isRunning = false;
    }

    // TODO
}

void Game::UpdateGame()
{
    // TODO
}

void Game::GenerateOutput()
{
    // set draw color blue
    SDL_SetRenderDrawColor(game->renderer,
                           0,    // R
                           0,    // G
                           255,  // B
                           255   // A
    );

    // clear back buffer
    SDL_RenderClear(game->renderer);

    // exchange front buffer for back buffer
    SDL_RenderPresent(game->renderer);
}
