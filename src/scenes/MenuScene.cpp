#include "MenuScene.h"
#include "../core/Game.h"
#include <iostream>

MenuScene::MenuScene(SDL_Renderer* renderer, TTF_Font* font, Game* game)
    : renderer(renderer), font(font), game(game) {
    buttons.emplace_back(300, 200, 200, 50, "Play", font, renderer, [this]() {
        std::cout << "Play button clicked\n";
        this->game->setState(Game::GameState::DECK_SELECTION);
        });
    buttons.emplace_back(300, 300, 200, 50, "Load/Continue", font, renderer, []() {
        std::cout << "Load/Continue button clicked (not implemented)\n";
        // Placeholder for future functionality
        });
    buttons.emplace_back(300, 400, 200, 50, "Options", font, renderer, []() {
        std::cout << "Options button clicked (not implemented)\n";
        // Placeholder for future functionality
        });
    buttons.emplace_back(300, 500, 200, 50, "Quit", font, renderer, [this]() {
        std::cout << "Quit button clicked\n";
        this->game->clean();
        exit(0);
        });
}

void MenuScene::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    for (auto& button : buttons) {
        button.render();
    }

    SDL_RenderPresent(renderer);
}

void MenuScene::handleEvent(SDL_Event& e) {
    for (auto& button : buttons) {
        button.handleEvent(e);
    }
}