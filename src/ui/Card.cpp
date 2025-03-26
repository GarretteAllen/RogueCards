#include "../includes/ui/Card.h"
#include "../includes/common/Constants.h"
#include <SDL_image.h>
#include <iostream>
#include <sstream>

Card::Card(int x, int y, const std::string& name, int damage, int energyCost, SDL_Renderer* renderer, TTF_Font* font, CardEffect effect)
    : rect{ x, y, Constants::CARD_WIDTH, Constants::CARD_HEIGHT }, originalRect{ x, y, Constants::CARD_WIDTH, Constants::CARD_HEIGHT },
    name(name), damage(damage), energyCost(energyCost), effect(effect), renderer(renderer), font(font),
    textTexture(nullptr), imageTexture(nullptr), isDragging(false), isHovered(false), isMagnified(false),
    hoverStartTime(0), needsTextTextureUpdate(true) {
}

void Card::setRenderer(SDL_Renderer* newRenderer) {
    renderer = newRenderer;
    if (imageTexture) {
        imageTexture = nullptr; // TextureManager will reload the image texture
    }
    if (textTexture) {
        textTexture = nullptr;
    }
    needsTextTextureUpdate = true;
}

void Card::setFont(TTF_Font* newFont) {
    font = newFont;
    if (textTexture) {
        textTexture = nullptr;
    }
    needsTextTextureUpdate = true;
}

void Card::createTextTexture() {
    if (textTexture) {
        textTexture = nullptr;
    }

    if (!renderer || !font) {
        std::cerr << "Renderer or font is null in createTextTexture for card: " << name << std::endl;
        return;
    }

    // Create a string with the card's details
    std::stringstream ss;
    ss << name << "\nDmg: " << damage << "\nCost: " << energyCost;
    std::string text = ss.str();

    SDL_Color textColor = { 255, 255, 255, 255 }; // White text
    SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), textColor, Constants::CARD_WIDTH - 10);
    if (!surface) {
        std::cerr << "Failed to create surface for card text: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "Failed to create texture for card text: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }

    textTexture = std::shared_ptr<SDL_Texture>(texture, SDL_DestroyTexture);
    SDL_FreeSurface(surface);
    needsTextTextureUpdate = false;
}

void Card::render(SDL_Renderer* renderer, int playerEnergy, int windowWidth, int windowHeight) {
    if (needsTextTextureUpdate && renderer && font) {
        createTextTexture();
    }

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

    if (textTexture) {
        int texW, texH;
        SDL_QueryTexture(textTexture.get(), nullptr, nullptr, &texW, &texH);
        SDL_Rect textRect = { renderRect.x + 5, renderRect.y + 5, texW, texH };
        SDL_RenderCopy(renderer, textTexture.get(), nullptr, &textRect);
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