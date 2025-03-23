#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "../systems/InputManager.h"
#include "entities/Enemy.h"
#include "../scenes/MenuScene.h"
#include "../scenes/BattleScene.h"
#include "../scenes/GameScene.h" // Add this
#include <memory>

enum class GameState { MENU, PLAYING, BATTLE };

class Game {
public:
    Game();
    ~Game();
    bool init(const char* title, int width, int height);
    void run();
    void clean();
    void setGameState(GameState newState);
    SDL_Renderer* getRenderer() const { return renderer; }
    TTF_Font* getFont() const { return font; }
    void startBattle(const Enemy& enemy); // Updated signature
    void endBattle(bool won);
    size_t currentNodeIndex;

private:
    bool isRunning;
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    InputManager inputManager;
    GameState currentState;
    std::unique_ptr<Scene> currentScene;

    void handleEvents();
    void update();
    void render();
};

#endif