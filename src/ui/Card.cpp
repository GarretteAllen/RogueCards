#include "Card.h"
#include <SDL_image.h>
#include <iostream>

Card::Card(int x, int y, const std::string& name, int damage, int energyCost, SDL_Renderer* renderer, TTF_Font* font, CardEffect effect)
    : rect{ x, y, 100, 150 }, originalRect{ x, y, 100, 150 }, name(name), damage(damage), energyCost(energyCost),
    effect(effect), textTexture(nullptr), imageTexture(nullptr), isDragging(false) {
    std::string cardText = name + " (" + std::to_string(energyCost) + "): " + std::to_string(damage);
    SDL_Color textColor = { 0, 0, 0, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, cardText.c_str(), textColor);
    if (surface) {
        textTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
}

void Card::render(SDL_Renderer* renderer) {
    if (imageTexture) {
        SDL_RenderCopy(renderer, imageTexture, nullptr, &rect);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    if (textTexture) {
        int texW, texH;
        SDL_QueryTexture(textTexture, nullptr, nullptr, &texW, &texH);
        SDL_Rect textRect = { rect.x + (rect.w - texW) / 2, rect.y + rect.h - texH - 5, texW, texH };
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    }
}

void Card::handleEvent(SDL_Event& e) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    bool inside = (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h);

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && inside) {
        isDragging = true;
    }

    if (e.type == SDL_MOUSEMOTION && isDragging) {
        rect.x = x - rect.w / 2;
        rect.y = y - rect.h / 2;
    }
}

void Card::loadImage(const std::string& path, SDL_Renderer* renderer) {
    if (imageTexture) {
        SDL_DestroyTexture(imageTexture);
        imageTexture = nullptr;
    }

    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "Failed to load image for card: " << name << " from " << path << " - IMG_Error: " << IMG_GetError() << std::endl;
        return;
    }

    imageTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (imageTexture) {
        std::cout << "Loaded image for card: " << name << " from " << path << std::endl;
    }
    else {
        std::cerr << "Failed to create texture for card: " << name << " from " << path << " - SDL_Error: " << SDL_GetError() << std::endl;
    }
}

void Card::resetPosition() {
    rect = originalRect;
}