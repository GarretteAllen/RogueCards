#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "Scene.h"
#include "../ui/Node.h"
#include "../core/Game.h"
#include <vector>

class GameScene : public Scene {
public:
    GameScene(SDL_Renderer* renderer, TTF_Font* font, Game* game);
    void update() override;
    void render(SDL_Renderer* renderer) override;
    void handleInput(SDL_Event* event) override;
    void unlockNextNode();

private:
    std::vector<Node> nodes;
    SDL_Renderer* renderer;
    TTF_Font* font;
    Game* game;
    int currentNodeIndex;

    void initializeNodes();
};

#endif