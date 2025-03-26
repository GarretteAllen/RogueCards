#include "../includes/scenes/OptionsScene.h"
#include "../includes/core/Game.h"
#include <iostream>

OptionsScene::OptionsScene(SDL_Renderer* renderer, TTF_Font* font, Game* game)
    : renderer(renderer), font(font), game(game), fullScreenButton(nullptr) {
    initializeButtons();
}

void OptionsScene::setRenderer(SDL_Renderer* newRenderer) {
    renderer = newRenderer;
    for (auto& button : buttons) {
        button.setRenderer(renderer);
    }
    updateFullScreenButton();
}

void OptionsScene::setFont(TTF_Font* newFont) {
    font = newFont;
    for (auto& button : buttons) {
        button.updateText(button.getLabel(), font, renderer);
    }
}

void OptionsScene::initializeButtons() {
    buttons.clear();
    int buttonWidth = 200;
    int buttonHeight = 50;
    int centerX = (game->getWindowWidth() - buttonWidth) / 2;

    buttons.emplace_back(
        centerX, 200, buttonWidth, buttonHeight, "800x600", font, renderer,
        [this]() {
            std::cout << "Set resolution to 800x600\n";
            game->setResolution(800, 600);
            initializeButtons();
        }
    );
    buttons.emplace_back(
        centerX, 300, buttonWidth, buttonHeight, "1200x800", font, renderer,
        [this]() {
            std::cout << "Set resolution to 1200x800\n";
            game->setResolution(1200, 800);
            initializeButtons();
        }
    );
    buttons.emplace_back(
        centerX, 400, buttonWidth, buttonHeight, "1920x1080", font, renderer,
        [this]() {
            std::cout << "Set resolution to 1920x1080\n";
            game->setResolution(1920, 1080);
            initializeButtons();
        }
    );

    std::string fullScreenText = game->isFullScreen() ? "Windowed" : "Full Screen";
    buttons.emplace_back(
        centerX, 500, buttonWidth, buttonHeight, fullScreenText, font, renderer,
        [this]() {
            std::cout << "Toggling full-screen mode\n";
            game->setFullScreen(!game->isFullScreen());
            updateFullScreenButton();
        }
    );
    fullScreenButton = &buttons.back();

    buttons.emplace_back(
        centerX, 600, buttonWidth, buttonHeight, "Back", font, renderer,
        [this]() {
            std::cout << "Returning to menu\n";
            game->setState(Game::GameState::MENU);
        }
    );
}

void OptionsScene::updateFullScreenButton() {
    if (fullScreenButton) {
        std::string fullScreenText = game->isFullScreen() ? "Windowed" : "Full Screen";
        fullScreenButton->updateText(fullScreenText, font, renderer);
        int buttonWidth = 200;
        int buttonHeight = 50;
        int centerX = (game->getWindowWidth() - buttonWidth) / 2;
        for (size_t i = 0; i < buttons.size(); ++i) {
            buttons[i].setPosition(centerX, 150 + i * 100);
        }
    }
}

void OptionsScene::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    for (auto& button : buttons) {
        button.render();
    }

    SDL_RenderPresent(renderer);
}

void OptionsScene::handleEvent(SDL_Event& e) {
    for (auto& button : buttons) {
        button.handleEvent(e);
    }
}