#ifndef MENU_SCENE_H
#define MENU_SCENE_H

#include "../ui/Button.h"
#include "Scene.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>

class Game;

class MenuScene : public Scene {
public:
    MenuScene(SDL_Renderer* renderer, TTF_Font* font, Game* game);
    void render() override;
    void handleEvent(SDL_Event& e) override;
    void setRenderer(SDL_Renderer* renderer) override;
    void setFont(TTF_Font* font) override;

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    Game* game;
    std::vector<Button> buttons;
};

#endif