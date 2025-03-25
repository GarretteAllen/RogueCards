#include "GameScene.h"
#include "../core/Game.h"
#include "../entities/Enemy.h"
#include <iostream>

GameScene::GameScene(SDL_Renderer* renderer, TTF_Font* font, Game* game)
    : renderer(renderer), font(font), game(game), currentNodeIndex(game->currentNodeIndex) {
    initializeNodes();
}

void GameScene::initializeNodes() {
    nodes.clear();
    // Goblin (index 0)
    nodes.push_back(Node(350, 250, 50, "Goblin", (currentNodeIndex == 0 ? 1.0f : 0.5f), renderer, font, [this]() {
        std::cout << "Starting Goblin Battle\n";
        Enemy goblin("Goblin", 10, 3);
        this->game->setState(Game::GameState::BATTLE);
        this->game->startBattle(goblin.name, goblin.hp, goblin.damage);
        }));
    // Ogre (index 1)
    nodes.push_back(Node(450, 250, 50, "Ogre", (currentNodeIndex == 1 ? 1.0f : 0.5f), renderer, font, [this]() {
        std::cout << "Starting Ogre Battle\n";
        Enemy ogre("Ogre", 20, 5);
        this->game->setState(Game::GameState::BATTLE);
        this->game->startBattle(ogre.name, ogre.hp, ogre.damage);
        }));
    // Troll (index 2)
    nodes.push_back(Node(550, 250, 50, "Troll", (currentNodeIndex == 2 ? 1.0f : 0.5f), renderer, font, [this]() {
        std::cout << "Starting Troll Battle\n";
        Enemy troll("Troll", 30, 4);
        this->game->setState(Game::GameState::BATTLE);
        this->game->startBattle(troll.name, troll.hp, troll.damage);
        }));

    
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (static_cast<int>(i) < currentNodeIndex) {
            nodes[i].isCompleted = true;
            nodes[i].opacity = 0.5f; 
        }
        else if (static_cast<int>(i) == currentNodeIndex) {
            nodes[i].isCompleted = false;
            nodes[i].opacity = 1.0f; 
        }
        else {
            nodes[i].isCompleted = false;
            nodes[i].opacity = 0.5f; 
        }
    }
}

void GameScene::unlockNextNode() {
    if (currentNodeIndex < static_cast<int>(nodes.size())) {
        nodes[currentNodeIndex].isCompleted = true;
        nodes[currentNodeIndex].opacity = 0.5f; 
        std::cout << "Completed " << nodes[currentNodeIndex].label << "\n";
        currentNodeIndex++;
        game->currentNodeIndex = currentNodeIndex;
    }
    if (currentNodeIndex < static_cast<int>(nodes.size())) {
        nodes[currentNodeIndex].opacity = 1.0f; 
        nodes[currentNodeIndex].isCompleted = false;
        std::cout << "Unlocked " << nodes[currentNodeIndex].label << "\n";
    }
}

void GameScene::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    for (const auto& node : nodes) {
        node.render();
    }

    SDL_RenderPresent(renderer);
}

void GameScene::handleEvent(SDL_Event& e) {
    for (auto& node : nodes) {
        node.handleEvent(e);
    }
}