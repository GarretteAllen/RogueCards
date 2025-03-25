#ifndef DECK_SELECTION_SCENE_H
#define DECK_SELECTION_SCENE_H

#include "Scene.h"
#include "../ui/Button.h"
#include <vector>

class Game;

class DeckSelectionScene : public Scene {
public:
    DeckSelectionScene(SDL_Renderer* renderer, TTF_Font* font, Game* game);
    void render() override;
    void handleEvent(SDL_Event& e) override;

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    Game* game;
    std::vector<Button> buttons;
};

#endif