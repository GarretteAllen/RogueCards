#ifndef DECK_SELECTION_SCENE_H
#define DECK_SELECTION_SCENE_H

#include "Scene.h"
#include "../ui/Button.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>

class Game;

class DeckSelectionScene : public Scene {
public:
	DeckSelectionScene(SDL_Renderer* renderer, TTF_Font* font, Game* game);
	void update() override;
	void render(SDL_Renderer* renderer) override;
	void handleInput(SDL_Event* event) override;

private:
	SDL_Renderer* renderer;
	TTF_Font* font;
	Game* game;
	std::vector<Button> buttons;
};

#endif