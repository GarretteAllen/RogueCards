#include "core/Game.h"
#include <SDL.h>

int main(int argc, char* argv[]) {
    Game game;
    if (!game.init("Roguelike Deckbuilder", 800, 600)) {
        return 1;
    }

    while (game.running()) {
        game.handleEvents();
        game.render();
        SDL_Delay(16);
    }

    return 0;
}
