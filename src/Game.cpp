#include "Game.h"
#include <algorithm>

#ifdef __EMSCRIPTEN__
    #include <SDL2/SDL_opengles2.h>
    #include <emscripten.h>
    #include <emscripten/html5.h>
#endif

Game* Game::game = nullptr;

Game::Game() : window(nullptr), isRunning(true), tickCount(0) {}

bool Game::Initialize()
{
    if (game)
    {
        SDL_Log("Game is already constructed!");
        exit(EXIT_FAILURE);
    }

    game = new Game();

    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
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

    std::string shaderName  = "sprite";
    std::string textureName = "example";

    if (!game->assetManager.LoadShader(shaderName, SPRITE_SHADER_VERT, SPRITE_SHADER_FRAG))
    {
        SDL_Log("Failed to load shader");
        return false;
    }

    game->assetManager.CreateSpriteVertex();

    if (!game->assetManager.LoadTexture(textureName, PLAYER_TEXTURE))
    {
        SDL_Log("Failed to load texture");
        return false;
    }

    if (TTF_Init() != 0)
    {
        SDL_Log("Failed to initialize SDL_ttf");
        return false;
    }

    game->assetManager.LoadFont(FONT_NAME, FONT_PATH);
    auto& font       = game->assetManager.GetFont(FONT_NAME);
    auto fontTexture = font.RenderText("Hello World !!", Font::DEFAULT_COLOR_WHITE, 40);
    game->assetManager.AddTexture("title", fontTexture);

    auto player = game->entityManager.AddEntity("player");
    player->AddComponent<StateComponent>();
    player->AddComponent<TransformComponent>(
        glm::vec2 {WINDOW_WIDTH / 2.0, WINDOW_HEIGHT / 2.0},  // position
        10.0f,                                                // scale
        200.0f                                                // speed
    );
    player->AddComponent<InputComponent>();
    player->AddComponent<SpriteComponent>(shaderName, textureName);
    game->player = player;

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

SDL_Window* Game::GetWindow()
{
    return window;
}

void Game::Stop()
{
    isRunning = false;
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

    auto& input = player->GetComponent<InputComponent>();
    input.Reset();

    if (state[SDL_SCANCODE_A])
    {
        input.left = true;
    }
    if (state[SDL_SCANCODE_D])
    {
        input.right = true;
    }
    if (state[SDL_SCANCODE_W])
    {
        input.up = true;
    }
    if (state[SDL_SCANCODE_S])
    {
        input.down = true;
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

    auto& input           = player->GetComponent<InputComponent>();
    auto& playerTransform = player->GetComponent<TransformComponent>();

    playerTransform.ResetVelocity();

    if (input.left)
    {
        playerTransform.velocity.x = -playerTransform.speed;
    }
    if (input.right)
    {
        playerTransform.velocity.x = playerTransform.speed;
    }
    if (input.up)
    {
        playerTransform.velocity.y = -playerTransform.speed;
    }
    if (input.down)
    {
        playerTransform.velocity.y = playerTransform.speed;
    }

    float playerEdge = player->GetComponent<RectComponent>().edge;

    playerTransform.position += playerTransform.velocity * deltaTime;
    playerTransform.position.x =
        std::clamp(playerTransform.position.x, 0.0f, Game::WINDOW_WIDTH - playerEdge);
    playerTransform.position.y =
        std::clamp(playerTransform.position.y, 0.0f, Game::WINDOW_HEIGHT - playerEdge);
}

void Game::GenerateOutput()
{
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);  // Set the clear color to grey
    glClear(GL_COLOR_BUFFER_BIT);          // Clear the color buffer

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // draw player
    auto& playerTransform = player->GetComponent<TransformComponent>();
    auto& playerSprite    = player->GetComponent<SpriteComponent>();

    auto& spriteShader = assetManager.GetShader(playerSprite.shaderName);
    auto& spriteVertex = assetManager.GetSpriteVertex();
    auto& texture      = assetManager.GetTexture(playerSprite.textureName);

    spriteShader.SetActive();
    spriteVertex.SetActive();

    spriteShader.SetVector2Uniform("uWindowSize", WINDOW_WIDTH, WINDOW_HEIGHT);
    spriteShader.SetVector2Uniform("uTextureSize", texture.GetWidth(), texture.GetHeight());
    spriteShader.SetVector2Uniform("uTexturePosition", playerTransform.position);
    spriteShader.SetFloatUniform("uTextureScale", playerTransform.scale);

    texture.SetActive();
    glDrawElements(GL_TRIANGLES, spriteVertex.GetNumIndices(), GL_UNSIGNED_INT, nullptr);

    // draw text
    auto& fontTexture = assetManager.GetTexture("title");
    spriteShader.SetVector2Uniform("uTextureSize", fontTexture.GetWidth(), fontTexture.GetHeight());
    spriteShader.SetVector2Uniform("uTexturePosition",
                                   glm::vec2 {WINDOW_WIDTH / 2.0f, fontTexture.GetHeight()});
    spriteShader.SetFloatUniform("uTextureScale", 3.0f);
    fontTexture.SetActive();
    glDrawElements(GL_TRIANGLES, spriteVertex.GetNumIndices(), GL_UNSIGNED_INT, nullptr);

    // swap the buffers
    SDL_GL_SwapWindow(window);
}
