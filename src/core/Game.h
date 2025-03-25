#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <memory>
#include "../scenes/Scene.h"
#include "../ui/Card.h"
#include "../entities/Enemy.h"
#include <vector>
#include <string>

class Game {
public:
    enum class GameState { MENU, DECK_SELECTION, GAME, BATTLE };
    enum class DeckType { DAMAGE, BALANCED, ELEMENTAL, DEFENSE };

    Game();
    ~Game();
    bool init(const char* title, int width, int height);
    void handleEvents();
    void render();
    void clean();
    void cleanup(); 
    bool running() const { return isRunning; }

    void setState(GameState newState);
    void selectDeck(DeckType deck);
    void startBattle(const std::string& enemyName, int enemyHP, int enemyDamage);
    void endBattle(bool won);
    const std::vector<Card>& getSelectedDeck() const { return selectedDeck; }

    int currentNodeIndex;

private:
    bool isRunning;
    bool isCleaned;
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    GameState currentState;
    std::unique_ptr<Scene> currentScene;
    DeckType selectedDeckType;
    std::vector<Card> selectedDeck;
};

#endif