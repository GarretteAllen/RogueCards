#ifndef OPTIONS_SCENE_H
#define OPTIONS_SCENE_H

#include "Scene.h"
#include "../ui/Button.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>

class Game;

class OptionsScene : public Scene {
public:
    OptionsScene(SDL_Renderer* renderer, TTF_Font* font, Game* game);
    void render() override;
    void handleEvent(SDL_Event& e) override;
    void setRenderer(SDL_Renderer* renderer) override;
    void setFont(TTF_Font* font) override; // New method

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    Game* game;
    std::vector<Button> buttons;
    Button* fullScreenButton;

    void initializeButtons();
    void updateFullScreenButton();
};

#endif