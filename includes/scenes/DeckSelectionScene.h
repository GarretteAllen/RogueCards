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
    void setRenderer(SDL_Renderer* renderer) override;
    void setFont(TTF_Font* font) override; // New method

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    Game* game;
    std::vector<Button> buttons;
};

#endif