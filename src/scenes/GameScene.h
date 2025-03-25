#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "Scene.h"
#include "../ui/Node.h"
#include <vector>

class Game;

class GameScene : public Scene {
public:
    GameScene(SDL_Renderer* renderer, TTF_Font* font, Game* game);
    void render() override;
    void handleEvent(SDL_Event& e) override;
    void unlockNextNode(); 

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    Game* game;
    std::vector<Node> nodes;
    int currentNodeIndex; 

    void initializeNodes();
};

#endif