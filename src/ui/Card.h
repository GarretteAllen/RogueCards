#ifndef CARD_H
#define CARD_H


#include "CardEffect.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class Card {
public:
	Card(int x, int y, const std::string& name, int damage, int energyCost,
		SDL_Renderer* renderer, TTF_Font* font, CardEffect effect = CardEffect());
	~Card();
	Card(Card&& other) noexcept;
	Card& operator=(Card&& other) noexcept;
	Card(const Card&) = delete;
	Card& operator=(const Card&) = delete;
	void render(SDL_Renderer* renderer);
	void handleEvent(SDL_Event* event);
	bool isDragging() const { return dragging; };
	SDL_Rect& getRect() { return rect; }
	const SDL_Rect& getRect() const { return rect; }
	int getDamage() const { return damage; }
	int getEnergyCost() const { return energyCost; }
	std::string getName() const { return name; }
	SDL_Rect& getOriginalRect() { return originalRect; }
	const SDL_Rect& getOriginalRect() const { return originalRect; }
	void resetPosition();
	CardEffect getEffect() const { return effect; }

private:
	SDL_Rect rect;
	SDL_Rect originalRect;
	std::string name;
	int damage;
	SDL_Renderer* renderer;
	TTF_Font* font;
	SDL_Texture* textTexture;
	bool dragging;
	int offsetX, offsetY;
	int energyCost;
	CardEffect effect;

	void updateTextTexture();
};

#endif