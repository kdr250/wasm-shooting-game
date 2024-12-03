#include "Game.h"

int main(int argc, char* argv[])
{
    bool success = Game::Initialize();
    if (!success)
    {
        return EXIT_FAILURE;
    }

    Game::RunLoop();

    return EXIT_SUCCESS;
}
