#include "Card.h"
#include <SDL_image.h>
#include <iostream>
#include <algorithm>

Card::Card(int x, int y, const std::string& name, int damage, int energyCost, SDL_Renderer* renderer, TTF_Font* font, CardEffect effect)
    : rect{ x, y, 100, 150 }, originalRect{ x, y, 100, 150 }, name(name), damage(damage), energyCost(energyCost),
    effect(effect), textTexture(nullptr), imageTexture(nullptr), isDragging(false), isHovered(false), isMagnified(false), renderer(renderer) {
}

Card::Card(const Card& other)
    : rect(other.rect), originalRect(other.originalRect), name(other.name), damage(other.damage), energyCost(other.energyCost),
    effect(other.effect), textTexture(nullptr), imageTexture(nullptr), isDragging(other.isDragging), isHovered(other.isHovered),
    isMagnified(other.isMagnified), renderer(other.renderer) {
    // Reload the image texture for the copied card using the correct convention
    if (other.imageTexture) {
        std::string lowercaseName = name;
        // Convert name to lowercase
        for (char& c : lowercaseName) {
            c = std::tolower(c);
        }
        // Remove spaces for file name
        std::replace(lowercaseName.begin(), lowercaseName.end(), ' ', '_');
        std::string imagePath = "assets/cards/" + lowercaseName + "_card.png";
        loadImage(imagePath, renderer);
    }
}

Card& Card::operator=(const Card& other) {
    if (this != &other) {
        // Destroy existing textures
        if (textTexture) {
            SDL_DestroyTexture(textTexture);
        }
        if (imageTexture) {
            SDL_DestroyTexture(imageTexture);
        }

        rect = other.rect;
        originalRect = other.originalRect;
        name = other.name;
        damage = other.damage;
        energyCost = other.energyCost;
        effect = other.effect;
        isDragging = other.isDragging;
        isHovered = other.isHovered;
        isMagnified = other.isMagnified;
        renderer = other.renderer;
        textTexture = nullptr;
        imageTexture = nullptr;

        // Reload the image texture for the copied card using the correct convention
        if (other.imageTexture) {
            std::string lowercaseName = name;
            // Convert name to lowercase
            for (char& c : lowercaseName) {
                c = std::tolower(c);
            }
            // Remove spaces for file name
            std::replace(lowercaseName.begin(), lowercaseName.end(), ' ', '_');
            std::string imagePath = "assets/cards/" + lowercaseName + "_card.png";
            loadImage(imagePath, renderer);
        }
    }
    return *this;
}

Card::~Card() {
    if (textTexture) {
        SDL_DestroyTexture(textTexture);
    }
    if (imageTexture) {
        SDL_DestroyTexture(imageTexture);
    }
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
        SDL_SetTextureAlphaMod(imageTexture, 128);
    }
    else {
        SDL_SetTextureAlphaMod(imageTexture, 255);
    }

    if (imageTexture) {
        SDL_RenderCopy(renderer, imageTexture, nullptr, &renderRect);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderFillRect(renderer, &renderRect);
    }

    SDL_SetTextureAlphaMod(imageTexture, 255);
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
    isMagnified = false;
}