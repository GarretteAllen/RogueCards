#include "core/Game.h"
#include "common/Constants.h"
#include <SDL.h>

int main(int argc, char* argv[]) {
    Game game;
    if (!game.init("Rogue Cards", Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT)) {
        return 1;
    }

    while (game.running()) {
        game.handleEvents();
        game.render();
        SDL_Delay(16);
    }

    return 0;
}
