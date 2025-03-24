#include "DeckSelectionScene.h"
#include "../core/Game.h"
#include <iostream>

DeckSelectionScene::DeckSelectionScene(SDL_Renderer* renderer, TTF_Font* font, Game* game)
    : renderer(renderer), font(font), game(game) {
    buttons.push_back(Button(300, 150, 200, 50, "Damage Deck", font, renderer, [this, game]() {
        std::cout << "Damage Deck selected\n";
        game->setSelectedDeck(0); // 0 = Damage Deck
        game->setGameState(GameState::PLAYING);
        }));
    buttons.push_back(Button(300, 250, 200, 50, "DoT Deck", font, renderer, [this, game]() {
        std::cout << "DoT Deck selected\n";
        game->setSelectedDeck(1); // 1 = DoT Deck
        game->setGameState(GameState::PLAYING);
        }));
    buttons.push_back(Button(300, 350, 200, 50, "Defense Deck", font, renderer, [this, game]() {
        std::cout << "Defense Deck selected\n";
        game->setSelectedDeck(2); // 2 = Defense Deck
        game->setGameState(GameState::PLAYING);
        }));
    buttons.push_back(Button(300, 450, 200, 50, "Balanced Deck", font, renderer, [this, game]() {
        std::cout << "Balanced Deck selected\n";
        game->setSelectedDeck(3); // 3 = Balanced Deck
        game->setGameState(GameState::PLAYING);
        }));
}

void DeckSelectionScene::update() {}

void DeckSelectionScene::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Dark gray background
    SDL_RenderClear(renderer);
    for (auto& button : buttons) {
        button.render(renderer);
    }
}

void DeckSelectionScene::handleInput(SDL_Event* event) {
    for (auto& button : buttons) {
        button.handleEvent(event);
    }
}