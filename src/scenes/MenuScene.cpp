#include "../includes/scenes/MenuScene.h"
#include "../includes/core/Game.h"
#include <iostream>

MenuScene::MenuScene(SDL_Renderer* renderer, TTF_Font* font, Game* game)
    : renderer(renderer), font(font), game(game) {
    buttons.emplace_back(
        (game->getWindowWidth() - 200) / 2, 200, 200, 50, "Play", font, renderer,
        [this]() {
            std::cout << "Play button clicked\n";
            this->game->setState(Game::GameState::DECK_SELECTION);
        }
    );
    buttons.emplace_back(
        (game->getWindowWidth() - 200) / 2, 300, 200, 50, "Load/Continue", font, renderer,
        []() {
            std::cout << "Load/Continue button clicked (not implemented)\n";
        }
    );
    buttons.emplace_back(
        (game->getWindowWidth() - 200) / 2, 400, 200, 50, "Options", font, renderer,
        [this]() {
            std::cout << "Options button clicked\n";
            this->game->setState(Game::GameState::OPTIONS);
        }
    );
    buttons.emplace_back(
        (game->getWindowWidth() - 200) / 2, 500, 200, 50, "Quit", font, renderer,
        [this]() {
            std::cout << "Quit button clicked\n";
            this->game->clean();
            exit(0);
        }
    );
}

void MenuScene::setRenderer(SDL_Renderer* newRenderer) {
    renderer = newRenderer;
    for (auto& button : buttons) {
        button.setRenderer(renderer);
    }
}

void MenuScene::setFont(TTF_Font* newFont) {
    font = newFont;
    for (auto& button : buttons) {
        button.updateText(button.getLabel(), font, renderer);
    }
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