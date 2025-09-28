#include <iostream>
#include "Game.h"

int main(int argc, char** argv) {
    (void)argc; (void)argv;

    Game game;

    bool success = game.Initialize();
    if (success) {
        game.RunLoop();
    }
    else {
        return 1;
    }

    game.Shutdown();
    return 0;
}
