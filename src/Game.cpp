#include "Game.h"
#include <SDL2/SDL_mixer.h>
#include <algorithm>
#include "scene/SceneMenu.h"

#ifdef __EMSCRIPTEN__
    #include <SDL2/SDL_opengles2.h>
    #include <emscripten.h>
    #include <emscripten/html5.h>
#endif

Game* Game::game = nullptr;

Game::Game() : window(nullptr), isRunning(true), tickCount(0), deltaTime(0.0f) {}

bool Game::Initialize()
{
    if (game)
    {
        SDL_Log("Game is already constructed!");
        exit(EXIT_FAILURE);
    }

    game = new Game();

    int sdlResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    if (sdlResult != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // Set OpenGL attributes
#ifdef __EMSCRIPTEN__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);      // Use the core OpenGL profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);  // Specify version 3.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);  // Request a color buffer with 8-bits per RGBA channel
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);        // Enable double buffering
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);  // Force OpenGL to use hardware acceleration

    game->window = SDL_CreateWindow("Hello world !!",         // window title
                                    SDL_WINDOWPOS_UNDEFINED,  // Top left x-coordinate of window
                                    SDL_WINDOWPOS_UNDEFINED,  // Top left y-coordinate of window
                                    Game::WINDOW_WIDTH,       // width of window
                                    Game::WINDOW_HEIGHT,      // height of window
                                    SDL_WINDOW_OPENGL);

    if (!game->window)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    // Create an OpenGL context
    game->context = SDL_GL_CreateContext(game->window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        SDL_Log("Failed to initialize GLEW.");
        return false;
    }

    glGetError();  // On some platforms, GLEW will emit a benign error code, so clear it

    if (TTF_Init() != 0)
    {
        SDL_Log("Failed to initialize SDL_ttf");
        return false;
    }

    if (Mix_Init(MIX_INIT_MP3) == 0)
    {
        SDL_Log("Failed to initialize SDL_mixer: %s", Mix_GetError());
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0)
    {
        SDL_Log("Failed to open audio: %s", Mix_GetError());
        return false;
    }

    // FIXME
    auto& soundManager = game->GetSoundManager();
    soundManager.LoadSound("bgm1", "resources/sound/bgm.mp3");
    soundManager.PlaySound("bgm1", true);

    game->ChangeScene("MENU", std::make_shared<SceneMenu>());

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

Game& Game::GetGame()
{
    return *game;
}

AssetManager& Game::GetAssetManager()
{
    return assetManager;
}

EntityManager& Game::GetEntityManger()
{
    return entityManager;
}

SoundManager& Game::GetSoundManager()
{
    return soundManager;
}

SDL_Window* Game::GetWindow()
{
    return window;
}

float Game::GetDeltaTime()
{
    return deltaTime;
}

long Game::ElapsedTimeSecond()
{
    return tickCount / 1000;
}

long Game::ElapsedTimeMillisecond()
{
    return tickCount;
}

long Game::SceneElapsedTimeSecond()
{
    return sceneMap[currentSceneName]->ElapsedTimeSecond();
}

long Game::SceneElapsedTimeMillisecond()
{
    return sceneMap[currentSceneName]->ElapsedTimeMillisecond();
}

void Game::Stop()
{
    isRunning = false;
}

void Game::ChangeScene(const std::string& sceneName,
                       std::shared_ptr<Scene> scene,
                       bool endCurrentScene)
{
    currentSceneName = sceneName;
    if (endCurrentScene)
    {
        sceneMap.erase(currentSceneName);
    }
    sceneMap.emplace(sceneName, scene);
}

void Game::OnSceneClear()
{
    sceneMap[currentSceneName]->OnClear();
}

void Game::OnSceneOver()
{
    sceneMap[currentSceneName]->OnOver();
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
    SDL_GL_DeleteContext(game->context);
    SDL_DestroyWindow(game->window);
    Mix_Quit();
    TTF_Quit();
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

            case SDL_KEYDOWN:
            case SDL_KEYUP:
            {
                action.type     = event.type == SDL_KEYDOWN ? "START" : "END";
                auto& actionMap = sceneMap[currentSceneName]->GetActionMap();
                auto iter       = actionMap.find(event.key.keysym.scancode);
                if (iter != actionMap.end())
                {
                    action.name = iter->second;
                    sceneMap[currentSceneName]->DoAction(action);
                }
                break;
            }

            default:
                break;
        }
    }
}

void Game::UpdateGame()
{
#ifndef __EMSCRIPTEN__
    // Wait until 16ms has elapsed since last frame
    while (!SDL_TICKS_PASSED(SDL_GetTicks64(), tickCount + 16))
        ;
#endif

    float delta = (SDL_GetTicks64() - tickCount) / 1000.0f;
    deltaTime   = std::min(delta, 0.05f);
    tickCount   = SDL_GetTicks64();

    auto& scene = sceneMap[currentSceneName];
    scene->Update(deltaTime);
}

void Game::GenerateOutput()
{
    auto& scene = sceneMap[currentSceneName];
    scene->Render();
}
