#ifndef CARD_H
#define CARD_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <iostream>

enum class CardEffectType { None, Armor, Heal, MultiStrike, Weaken, Poison, Thorns, Wet, Lightning, Ice };

struct CardEffect {
    CardEffectType type;
    int value;
    int count;

    CardEffect(CardEffectType t = CardEffectType::None, int v = 0, int c = 0) : type(t), value(v), count(c) {}
};

class Card {
public:
    Card(int x, int y, const std::string& name, int damage, int energyCost, SDL_Renderer* renderer, TTF_Font* font, CardEffect effect = CardEffect());
    void render(SDL_Renderer* renderer);
    void handleEvent(SDL_Event& e);
    void loadImage(const std::string& path, SDL_Renderer* renderer);
    void resetPosition();

    SDL_Rect& getRect() { return rect; }
    SDL_Rect& getOriginalRect() {
        std::cout << "Modifying originalRect for card: " << name << "\n";
        return originalRect;
    }
    std::string getName() const { return name; }
    int getDamage() const { return damage; }
    int getEnergyCost() const { return energyCost; }
    CardEffect getEffect() const { return effect; }

    bool isDragging;

private:
    SDL_Rect rect;
    SDL_Rect originalRect;
    std::string name;
    int damage;
    int energyCost;
    CardEffect effect;
    SDL_Texture* textTexture;
    SDL_Texture* imageTexture;
};

#endif