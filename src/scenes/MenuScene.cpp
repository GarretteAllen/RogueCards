#include "MenuScene.h"
#include <iostream>
#include "../core/Game.h"

MenuScene::MenuScene(SDL_Renderer* renderer, TTF_Font* font, Game* game)
    : renderer(renderer), font(font), game(game) {
    buttons.push_back(Button(300, 200, 200, 50, "Play", font, renderer, [this, game]() {
        std::cout << "Play button clicked\n";
        game->setGameState(GameState::PLAYING);
        }));
    buttons.push_back(Button(300, 300, 200, 50, "Options", font, renderer, []() {
        std::cout << "Options button clicked\n";
        }));
    buttons.push_back(Button(300, 400, 200, 50, "Quit", font, renderer, []() {
        exit(0);
        }));
}

void MenuScene::update() {

}

void MenuScene::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (auto& button : buttons) {
        button.render(renderer);
    }
}

void MenuScene::handleInput(SDL_Event* event) {
    for (auto& button : buttons) {
        button.handleEvent(event);
    }
}