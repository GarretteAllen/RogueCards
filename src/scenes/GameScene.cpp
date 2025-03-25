#include "GameScene.h"
#include "../core/Game.h"
#include "../entities/Enemy.h"
#include "../scenes/RewardScene.h"
#include <iostream>
#include <algorithm>

GameScene::GameScene(SDL_Renderer* renderer, TTF_Font* font, Game* game)
    : renderer(renderer), font(font), game(game), currentNodeIndex(game->currentNodeIndex), gameOver(false) {
    lockedNodes.clear();
    if (game->completedNodes.empty()) {
        game->completedNodes = std::vector<bool>(6, false); // Updated for 6 nodes (added Dragon)
    }
    initializeNodes();
    updateActiveNodes();
}

void GameScene::initializeNodes() {
    nodes.clear();

    nodes.emplace_back(
        400 - 25, 500 - 25, 50, "Goblin",
        0.5f,
        renderer, font,
        [this]() {
            std::cout << "Starting Goblin Battle\n";
            Enemy goblin("Goblin", 10, 3);
            this->game->setState(Game::GameState::BATTLE);
            this->game->startBattle(goblin.name, goblin.hp, goblin.damage);
        },
        NodeType::Fight,
        SDL_Color{ 200, 200, 200, 255 },
        std::vector<int>{1, 2}
    );

    nodes.emplace_back(
        500 - 25, 400 - 25, 50, "Green Reward",
        0.5f,
        renderer, font,
        [this]() {
            std::cout << "Entering Green Reward\n";
            game->setRewardScene(std::make_unique<RewardScene>(renderer, font, game, RewardScene::RewardType::Green));
            game->setState(Game::GameState::REWARD);
            markNodeAsCompleted(currentNodeIndex);
            lockSiblingNodes(currentNodeIndex);
        },
        NodeType::Reward,
        SDL_Color{ 0, 255, 0, 255 },
        std::vector<int>{4}
    );

    nodes.emplace_back(
        300 - 25, 400 - 25, 50, "Troll",
        0.5f,
        renderer, font,
        [this]() {
            std::cout << "Starting Troll Battle\n";
            Enemy troll("Troll", 30, 4);
            this->game->setState(Game::GameState::BATTLE);
            this->game->startBattle(troll.name, troll.hp, troll.damage);
        },
        NodeType::Fight,
        SDL_Color{ 255, 0, 0, 255 },
        std::vector<int>{3}
    );

    nodes.emplace_back(
        300 - 25, 300 - 25, 50, "Purple Reward",
        0.5f,
        renderer, font,
        [this]() {
            std::cout << "Entering Purple Reward\n";
            game->setRewardScene(std::make_unique<RewardScene>(renderer, font, game, RewardScene::RewardType::Purple));
            game->setState(Game::GameState::REWARD);
            markNodeAsCompleted(currentNodeIndex);
        },
        NodeType::Reward,
        SDL_Color{ 128, 0, 128, 255 },
        std::vector<int>{5}
    );

    nodes.emplace_back(
        500 - 25, 300 - 25, 50, "Ogre",
        0.5f,
        renderer, font,
        [this]() {
            std::cout << "Starting Ogre Battle\n";
            Enemy ogre("Ogre", 20, 5);
            this->game->setState(Game::GameState::BATTLE);
            this->game->startBattle(ogre.name, ogre.hp, ogre.damage);
        },
        NodeType::Fight,
        SDL_Color{ 255, 165, 0, 255 },
        std::vector<int>{}
    );

    nodes.emplace_back(
        300 - 25, 200 - 25, 50, "Dragon",
        0.5f,
        renderer, font,
        [this]() {
            std::cout << "Starting Dragon Battle\n";
            Enemy dragon("Dragon", 50, 6);
            this->game->setState(Game::GameState::BATTLE);
            this->game->startBattle(dragon.name, dragon.hp, dragon.damage);
        },
        NodeType::Fight,
        SDL_Color{ 255, 0, 0, 255 },
        std::vector<int>{}
    );

    if (game->completedNodes.size() < nodes.size()) {
        game->completedNodes.resize(nodes.size(), false);
    }

    for (size_t i = 0; i < game->completedNodes.size() && i < nodes.size(); ++i) {
        if (game->completedNodes[i]) {
            nodes[i].isCompleted = true;
            nodes[i].opacity = 0.5f;
        }
    }
}

void GameScene::updateActiveNodes() {
    std::vector<int> previousActiveNodes = activeNodes;
    activeNodes.clear();

    bool anyCompleted = false;
    for (size_t i = 0; i < game->completedNodes.size(); ++i) {
        if (game->completedNodes[i]) {
            anyCompleted = true;
            break;
        }
    }

    if (!anyCompleted) {
        activeNodes.push_back(0);
        currentNodeIndex = 0;
        game->currentNodeIndex = currentNodeIndex;
        std::cout << "Unlocked " << nodes[0].label << "\n";
    }
    else {
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (game->completedNodes[i]) {
                continue;
            }
            if (std::find(lockedNodes.begin(), lockedNodes.end(), i) != lockedNodes.end()) {
                continue;
            }
            for (size_t j = 0; j < nodes.size(); ++j) {
                if (game->completedNodes[j]) {
                    for (int nextIndex : nodes[j].nextNodes) {
                        if (nextIndex == static_cast<int>(i)) {
                            if (std::find(activeNodes.begin(), activeNodes.end(), i) == activeNodes.end()) {
                                activeNodes.push_back(i);
                                std::cout << "Unlocked " << nodes[i].label << "\n";
                            }
                        }
                    }
                }
            }
        }
    }

    for (size_t i = 0; i < nodes.size(); ++i) {
        if (i < game->completedNodes.size() && game->completedNodes[i]) {
            nodes[i].isCompleted = true;
            nodes[i].opacity = 0.5f;
        }
        else if (std::find(activeNodes.begin(), activeNodes.end(), i) != activeNodes.end()) {
            nodes[i].isCompleted = false;
            nodes[i].opacity = 1.0f;
        }
        else {
            nodes[i].isCompleted = false;
            nodes[i].opacity = 0.5f;
        }
    }

    if (!activeNodes.empty()) {
        currentNodeIndex = *std::min_element(activeNodes.begin(), activeNodes.end());
        game->currentNodeIndex = currentNodeIndex;
    }
    else {
        currentNodeIndex = static_cast<int>(nodes.size());
        game->currentNodeIndex = currentNodeIndex;
        std::cout << "Game Over: No more nodes to explore!\n";
        gameOver = true;
    }

    std::cout << "Previous active nodes: ";
    for (int idx : previousActiveNodes) {
        std::cout << idx << " ";
    }
    std::cout << "\nNew active nodes: ";
    for (int idx : activeNodes) {
        std::cout << idx << " ";
    }
    std::cout << "\nCurrent node index: " << currentNodeIndex << "\n";
}

void GameScene::markNodeAsCompleted(int nodeIndex) {
    if (nodeIndex >= 0 && nodeIndex < static_cast<int>(nodes.size()) && nodeIndex < static_cast<int>(game->completedNodes.size())) {
        nodes[nodeIndex].isCompleted = true;
        nodes[nodeIndex].opacity = 0.5f;
        game->completedNodes[nodeIndex] = true;
        std::cout << "Completed " << nodes[nodeIndex].label << "\n";
    }
}

void GameScene::lockSiblingNodes(int completedNodeIndex) {
    if (completedNodeIndex == 2) { // Troll node
        activeNodes.erase(
            std::remove(activeNodes.begin(), activeNodes.end(), 1),
            activeNodes.end()
        );
        if (std::find(lockedNodes.begin(), lockedNodes.end(), 1) == lockedNodes.end()) {
            lockedNodes.push_back(1); // Lock Green Reward
        }
    }
    else if (completedNodeIndex == 1) { // Green Reward node
        activeNodes.erase(
            std::remove(activeNodes.begin(), activeNodes.end(), 2),
            activeNodes.end()
        );
        if (std::find(lockedNodes.begin(), lockedNodes.end(), 2) == lockedNodes.end()) {
            lockedNodes.push_back(2); // Lock Troll
        }
    }
}

void GameScene::updateProgression() {
    initializeNodes();
    updateActiveNodes();
}

void GameScene::unlockNextNode() {
    if (currentNodeIndex < static_cast<int>(nodes.size())) {
        markNodeAsCompleted(currentNodeIndex);
        updateProgression();
    }
}

void GameScene::render() {
    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (size_t i = 0; i < nodes.size(); ++i) {
        const Node& node = nodes[i];
        for (int nextIndex : node.nextNodes) {
            if (nextIndex >= 0 && nextIndex < static_cast<int>(nodes.size())) {
                const Node& nextNode = nodes[nextIndex];
                int startX = node.rect.x + node.rect.w / 2;
                int startY = node.rect.y + node.rect.h / 2;
                int endX = nextNode.rect.x + nextNode.rect.w / 2;
                int endY = nextNode.rect.y + nextNode.rect.h / 2;
                SDL_RenderDrawLine(renderer, startX, startY, endX, endY);
            }
        }
    }

    for (size_t i = 0; i < nodes.size(); ++i) {
        const Node& node = nodes[i];
        node.render();
    }

    SDL_RenderPresent(renderer);
}

void GameScene::handleEvent(SDL_Event& e) {
    for (size_t i = 0; i < nodes.size(); ++i) {
        bool isActive = std::find(activeNodes.begin(), activeNodes.end(), i) != activeNodes.end();
        bool isLocked = std::find(lockedNodes.begin(), lockedNodes.end(), i) != lockedNodes.end();
        if (isActive && !isLocked && !nodes[i].isCompleted) {
            if (nodes[i].handleEvent(e)) {
                currentNodeIndex = static_cast<int>(i);
                game->currentNodeIndex = currentNodeIndex;
                break;
            }
        }
    }
}