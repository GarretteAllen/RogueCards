#include "../includes/scenes/DeckSelectionScene.h"
#include "../includes/core/Game.h"
#include <iostream>

DeckSelectionScene::DeckSelectionScene(SDL_Renderer* renderer, TTF_Font* font, Game* game)
    : renderer(renderer), font(font), game(game) {
    buttons.push_back(Button(300, 150, 200, 50, "Damage Deck", font, renderer, [this, game]() {
        std::cout << "Damage Deck selected\n";
        game->selectDeck(Game::DeckType::DAMAGE);
        game->setState(Game::GameState::GAME);
        }));
    buttons.push_back(Button(300, 250, 200, 50, "Elemental Deck", font, renderer, [this, game]() {
        std::cout << "Elemental Deck selected\n";
        game->selectDeck(Game::DeckType::ELEMENTAL);
        game->setState(Game::GameState::GAME);
        }));
    buttons.push_back(Button(300, 350, 200, 50, "Defense Deck", font, renderer, [this, game]() {
        std::cout << "Defense Deck selected\n";
        game->selectDeck(Game::DeckType::DEFENSE);
        game->setState(Game::GameState::GAME);
        }));
    buttons.push_back(Button(300, 450, 200, 50, "Balanced Deck", font, renderer, [this, game]() {
        std::cout << "Balanced Deck selected\n";
        game->selectDeck(Game::DeckType::BALANCED);
        game->setState(Game::GameState::GAME);
        }));
}

void DeckSelectionScene::setRenderer(SDL_Renderer* newRenderer) {
    renderer = newRenderer;
    for (auto& button : buttons) {
        button.setRenderer(renderer);
    }
}

void DeckSelectionScene::setFont(TTF_Font* newFont) {
    font = newFont;
    for (auto& button : buttons) {
        button.updateText(button.getLabel(), font, renderer);
    }
}

void DeckSelectionScene::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    for (auto& button : buttons) {
        button.render();
    }

    SDL_RenderPresent(renderer);
}

void DeckSelectionScene::handleEvent(SDL_Event& e) {
    for (auto& button : buttons) {
        button.handleEvent(e);
    }
}