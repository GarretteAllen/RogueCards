#include "core/Game.h"

int main(int argc, char* argv[]) {
    Game game;
    if (!game.init("Roguelike Deckbuilder", 800, 600)) {
        return -1;
    }
    game.run();
    return 0;
}
