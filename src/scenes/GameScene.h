#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "Scene.h"
#include "../ui/Node.h"
#include <vector>
#include <functional>

class Game;

class GameScene : public Scene {
public:
    GameScene(SDL_Renderer* renderer, TTF_Font* font, Game* game);
    void render() override;
    void handleEvent(SDL_Event& e) override;

    void markNodeAsCompleted(int nodeIndex);
    void lockSiblingNodes(int completedNodeIndex);
    void updateProgression();
    bool isGameOver() const { return gameOver; }

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    Game* game;
    std::vector<Node> nodes;
    std::vector<int> activeNodes;
    std::vector<int> lockedNodes;
    int currentNodeIndex;
    bool gameOver;

    void initializeNodes();
    void updateActiveNodes();
    void unlockNextNode();
};

#endif