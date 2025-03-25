#include "Card.h"
#include "../common/Constants.h"
#include <SDL_image.h>
#include <iostream>

Card::Card(int x, int y, const std::string& name, int damage, int energyCost, SDL_Renderer* renderer, TTF_Font* font, CardEffect effect)
    : rect{ x, y, Constants::CARD_WIDTH, Constants::CARD_HEIGHT }, originalRect{ x, y, Constants::CARD_WIDTH, Constants::CARD_HEIGHT }, name(name), damage(damage), energyCost(energyCost),
    effect(effect), textTexture(nullptr), imageTexture(nullptr), isDragging(false), isHovered(false), isMagnified(false), hoverStartTime() {
}

void Card::render(SDL_Renderer* renderer, int playerEnergy, int windowWidth, int windowHeight) {
    SDL_Rect renderRect = rect;
    if (isMagnified && !isDragging) {
        renderRect.w = originalRect.w * 1.5;
        renderRect.h = originalRect.h * 1.5;

        renderRect.x = rect.x - (renderRect.w - rect.w) / 2;
        renderRect.y = rect.y - (renderRect.h - rect.h) / 2;

        if (renderRect.x < 0) {
            renderRect.x = 0;
        }
        if (renderRect.y < 0) {
            renderRect.y = 0;
        }
        if (renderRect.x + renderRect.w > windowWidth) {
            renderRect.x = windowWidth - renderRect.w;
        }
        if (renderRect.y + renderRect.h > windowHeight) {
            renderRect.y = windowHeight - renderRect.h;
        }
    }

    if (playerEnergy < energyCost && !isDragging) {
        SDL_SetTextureAlphaMod(imageTexture.get(), 128);
    }
    else {
        SDL_SetTextureAlphaMod(imageTexture.get(), 255);
    }

    if (imageTexture) {
        SDL_RenderCopy(renderer, imageTexture.get(), nullptr, &renderRect);
    }
    else {
        SDL_SetRenderDrawColor(renderer, Constants::COLOR_GRAY.r, Constants::COLOR_GRAY.g, Constants::COLOR_GRAY.b, Constants::COLOR_GRAY.a);
        SDL_RenderFillRect(renderer, &renderRect);
    }

    SDL_SetTextureAlphaMod(imageTexture.get(), 255);
}

void Card::handleEvent(SDL_Event& e) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    bool inside = (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h);

    if (e.type == SDL_MOUSEMOTION) {
        if (inside && !isDragging) {
            if (!isHovered) {
                isHovered = true;
                hoverStartTime = SDL_GetTicks();
            }

            if (!isMagnified && (SDL_GetTicks() - hoverStartTime >= HOVER_DELAY)) {
                isMagnified = true;
            }
        }
        else {
            isHovered = false;
            isMagnified = false;
        }
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && inside) {
        isDragging = true;
        isMagnified = false;
    }

    if (e.type == SDL_MOUSEMOTION && isDragging) {
        rect.x = x - rect.w / 2;
        rect.y = y - rect.h / 2;
    }
}

void Card::loadImage(const std::string& path, SDL_Renderer* renderer, TextureManager& textureManager) {
    imageTexture = textureManager.loadTexture(path, renderer);
}

void Card::resetPosition() {
    rect = originalRect;
    isMagnified = false;
}