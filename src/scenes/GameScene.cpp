#include "GameScene.h"
#include <iostream>

GameScene::GameScene(SDL_Renderer* renderer, TTF_Font* font, Game* game)
    : renderer(renderer), font(font), game(game), currentNodeIndex(game->currentNodeIndex) {
    initializeNodes();
}

void GameScene::initializeNodes() {
    nodes.clear();
    nodes.push_back(Node(350, 250, 50, "Goblin", (currentNodeIndex == 0 ? 1.0f : (currentNodeIndex > 0 ? 0.3f : 0.5f)), renderer, font, [this]() {
        std::cout << "Starting Goblin Battle\n";
        Enemy goblin("Goblin", 10, 3);
        game->setGameState(GameState::BATTLE);
        game->startBattle(goblin);
        }));
    nodes.push_back(Node(450, 250, 50, "Ogre", (currentNodeIndex == 1 ? 1.0f : (currentNodeIndex > 1 ? 0.3f : 0.5f)), renderer, font, [this]() {
        std::cout << "Starting Ogre Battle\n";
        Enemy ogre("Ogre", 20, 5);
        game->setGameState(GameState::BATTLE);
        game->startBattle(ogre);
        }));
    nodes.push_back(Node(550, 250, 50, "Troll", (currentNodeIndex == 2 ? 1.0f : (currentNodeIndex > 2 ? 0.3f : 0.5f)), renderer, font, [this]() {
        std::cout << "Starting Troll Battle\n";
        Enemy troll("Troll", 30, 4);
        game->setGameState(GameState::BATTLE);
        game->startBattle(troll);
        }));
    // Set completed state for nodes before currentNodeIndex
    for (size_t i = 0; i < currentNodeIndex && i < nodes.size(); ++i) {
        nodes[i].isCompleted = true;
    }
}

void GameScene::unlockNextNode() {
    // Removed increment here; handled by Game::endBattle
    if (currentNodeIndex < nodes.size()) {
        nodes[currentNodeIndex].isCompleted = true; // Mark current as completed
        nodes[currentNodeIndex].opacity = 0.3f; // Dim completed node
        std::cout << "Completed " << nodes[currentNodeIndex].label << "\n";
    }
    if (currentNodeIndex + 1 < nodes.size()) {
        nodes[currentNodeIndex + 1].opacity = 1.0f; // Unlock next node
        std::cout << "Unlocked " << nodes[currentNodeIndex + 1].label << "\n";
    }
}

void GameScene::update() {
    // Update game objects if needed
}

void GameScene::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue background
    SDL_RenderClear(renderer);

    for (auto& node : nodes) {
        node.render(renderer);
    }
}

void GameScene::handleInput(SDL_Event* event) {
    if (event->type == SDL_MOUSEMOTION) {
        for (auto& node : nodes) {
            node.isHovered = false;
        }
    }

    for (auto& node : nodes) {
        node.handleEvent(event);
    }
    if (event->type == SDL_KEYDOWN) {
        std::cout << "Key pressed in game scene" << std::endl;
    }
}