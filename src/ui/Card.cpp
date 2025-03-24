#include "Card.h"
#include <iostream>

Card::Card(int x, int y, const std::string& name, int damage, int energyCost,
    SDL_Renderer* renderer, TTF_Font* font, CardEffect effect)
    : name(name), damage(damage), energyCost(energyCost), effect(effect),
    renderer(renderer), font(font), dragging(false), offsetX(0), offsetY(0), textTexture(nullptr) {
    rect = { x, y, 100, 150 };
    originalRect = rect;
    std::cout << "Card created: " << name << ", dmg: " << damage << ", cost: " << energyCost
        << ", effect: " << (int)effect.type << ", value: " << effect.value << ", count: " << effect.count << std::endl;
    updateTextTexture();
}

Card::~Card() {
    if (textTexture) {
        SDL_DestroyTexture(textTexture);
    }
}

Card::Card(Card&& other) noexcept
    : rect(other.rect), originalRect(other.originalRect), name(std::move(other.name)),
    damage(other.damage), energyCost(other.energyCost), effect(other.effect),
    renderer(other.renderer), font(other.font), textTexture(other.textTexture),
    dragging(other.dragging), offsetX(other.offsetX), offsetY(other.offsetY) {
    other.textTexture = nullptr;
    other.effect = CardEffect();
}

Card& Card::operator=(Card&& other) noexcept {
    if (this != &other) {
        if (textTexture) SDL_DestroyTexture(textTexture);
        rect = other.rect;
        originalRect = other.originalRect;
        name = std::move(other.name);
        damage = other.damage;
        energyCost = other.energyCost;
        effect = other.effect;
        renderer = other.renderer;
        font = other.font;
        textTexture = other.textTexture;
        dragging = other.dragging;
        offsetX = other.offsetX;
        offsetY = other.offsetY;
        other.textTexture = nullptr;
        other.effect = CardEffect();
    }
    return *this;
}

void Card::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 200, 255); 
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
    SDL_RenderDrawRect(renderer, &rect);

    if (textTexture) {
        int w, h;
        SDL_QueryTexture(textTexture, nullptr, nullptr, &w, &h);
        SDL_Rect textRect = { rect.x + 10, rect.y + 10, w, h };
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    }
}

void Card::handleEvent(SDL_Event* event) {
    int mouseX, mouseY;
    switch (event->type) {
    case SDL_MOUSEBUTTONDOWN:
        mouseX = event->button.x;
        mouseY = event->button.y;
        if (mouseX >= rect.x && mouseX <= rect.x + rect.w &&
            mouseY >= rect.y && mouseY <= rect.y + rect.h) {
            dragging = true;
            offsetX = mouseX - rect.x;
            offsetY = mouseY - rect.y;
            std::cout << "Started dragging: " << name << std::endl;
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if (dragging) {
            dragging = false;
            std::cout << "Stopped dragging: " << name << " at (" << rect.x << ", " << rect.y << ")" << std::endl;
        }
        break;
    case SDL_MOUSEMOTION:
        if (dragging) {
            mouseX = event->motion.x;
            mouseY = event->motion.y;
            rect.x = mouseX - offsetX;
            rect.y = mouseY - offsetY;
        }
        break;
    }
}

void Card::resetPosition() {
    rect = originalRect;
    dragging = false;
}

void Card::updateTextTexture() {
    if (textTexture) SDL_DestroyTexture(textTexture);
    textTexture = nullptr;
    if (!renderer || !font) {
        std::cerr << "Renderer or font null for " << name << std::endl;
        return;
    }
    std::string effectStr;
    switch (effect.type) {
    case CardEffectType::Armor:
        effectStr = " Armor:" + std::to_string(effect.value);
        break;
    case CardEffectType::Heal:
        effectStr = " Heal:" + std::to_string(effect.value);
        break;
    case CardEffectType::MultiStrike:
        effectStr = " x" + std::to_string(effect.count) + ":" + std::to_string(effect.value);
        break;
    case CardEffectType::Weaken:
        effectStr = " Weaken:" + std::to_string(effect.value) + " (" + std::to_string(effect.count) + "t)";
        break;
    case CardEffectType::Poison:
        effectStr = " Poison:" + std::to_string(effect.value) + " (" + std::to_string(effect.count) + "t";
        break;
    case CardEffectType::Thorns:
        effectStr = " Thorns";
        break;
    default:
        break;
    }
    std::string displayText = name + "\nDmg:" + std::to_string(damage) + " Cost:" + std::to_string(energyCost) + effectStr;
    SDL_Color textColor = { 0, 0, 0, 255 };
    SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, displayText.c_str(), textColor, 90);
    if (!surface) {
        std::cerr << "Failed to create surface for " << name << ": " << TTF_GetError() << std::endl;
        return;
    }
    textTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    std::cout << "Text for " << name << ": " << displayText << std::endl;
}