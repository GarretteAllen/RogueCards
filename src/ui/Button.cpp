#include "Button.h"
#include <iostream>

Button::Button(int x, int y, int w, int h, const std::string& label, TTF_Font* font, SDL_Renderer* renderer,
    std::function<void()> onClick)
    : rect{ x, y, w, h }, originalRect{ x, y, w, h }, label(label), font(font), renderer(renderer), texture(nullptr),
    onClick(onClick), hovered(false), needsTextureUpdate(true) {
    // Defer texture creation to render()
}

Button::~Button() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

Button::Button(Button&& other) noexcept
    : rect(other.rect), originalRect(other.originalRect), label(std::move(other.label)), font(other.font),
    renderer(other.renderer), texture(other.texture), onClick(std::move(other.onClick)), hovered(other.hovered),
    needsTextureUpdate(other.needsTextureUpdate) {
    other.texture = nullptr;
    other.hovered = false;
    other.needsTextureUpdate = false;
}

Button& Button::operator=(Button&& other) noexcept {
    if (this != &other) {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
        rect = other.rect;
        originalRect = other.originalRect;
        label = std::move(other.label);
        font = other.font;
        renderer = other.renderer;
        texture = other.texture;
        onClick = std::move(other.onClick);
        hovered = other.hovered;
        needsTextureUpdate = other.needsTextureUpdate;
        other.texture = nullptr;
        other.hovered = false;
        other.needsTextureUpdate = false;
    }
    return *this;
}

void Button::setRenderer(SDL_Renderer* newRenderer) {
    renderer = newRenderer;
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    needsTextureUpdate = true; // Defer texture creation to render()
}

void Button::createTexture() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    if (!renderer) {
        std::cerr << "Renderer is null in createTexture for button: " << label << std::endl;
        return;
    }

    if (!font) {
        std::cerr << "Font is null in createTexture for button: " << label << std::endl;
        return;
    }

    SDL_Color textColor = { 0, 0, 0, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, label.c_str(), textColor);
    if (!surface) {
        std::cerr << "Failed to create surface for button label: " << TTF_GetError() << std::endl;
        return;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "Failed to create texture for button label: " << SDL_GetError() << std::endl;
    }

    SDL_FreeSurface(surface);
    needsTextureUpdate = false;
}

void Button::updateText(const std::string& newLabel, TTF_Font* font, SDL_Renderer* renderer) {
    label = newLabel;
    this->font = font;
    this->renderer = renderer;
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    needsTextureUpdate = true; // Defer texture creation to render()
}

void Button::render() {
    if (needsTextureUpdate && renderer && font) {
        createTexture();
    }

    if (hovered) {
        SDL_SetRenderDrawColor(renderer, 100, 255, 100, 255); // Light green when hovered
        rect.w = originalRect.w * 1.1;
        rect.h = originalRect.h * 1.1;
        rect.x = originalRect.x - (rect.w - originalRect.w) / 2;
        rect.y = originalRect.y - (rect.h - originalRect.h) / 2;
    }
    else {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Gray when not hovered
        rect = originalRect;
    }
    SDL_RenderFillRect(renderer, &rect);

    if (texture) {
        int texW, texH;
        SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);
        SDL_Rect textRect = { rect.x + (rect.w - texW) / 2, rect.y + (rect.h - texH) / 2, texW, texH };
        SDL_RenderCopy(renderer, texture, nullptr, &textRect);
    }
}

void Button::handleEvent(SDL_Event& e) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    bool inside = (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h);

    // Update hover state
    if (e.type == SDL_MOUSEMOTION) {
        hovered = inside;
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && inside) {
        onClick();
    }
}

void Button::setPosition(int x, int y) {
    rect.x = x;
    rect.y = y;
    originalRect.x = x;
    originalRect.y = y;
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    needsTextureUpdate = true; // Defer texture creation to render()
}