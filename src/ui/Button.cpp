#include "Button.h"
#include <iostream>

Button::Button(int x, int y, int w, int h, const std::string& label, TTF_Font* font, SDL_Renderer* renderer,
    std::function<void()> onClick)
    : rect{ x, y, w, h }, label(label), texture(nullptr), renderer(renderer), onClick(onClick) {
    SDL_Color textColor = { 0, 0, 0, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, label.c_str(), textColor);
    if (!surface) {
        std::cerr << "Failed to create surface for button: " << TTF_GetError() << std::endl;
        return;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

Button::~Button() {
    if (texture) SDL_DestroyTexture(texture);
}

Button::Button(Button&& other) noexcept
    : rect(other.rect), label(std::move(other.label)), font(other.font), renderer(other.renderer),
    texture(other.texture), onClick(std::move(other.onClick)), hovered(other.hovered) {
    other.texture = nullptr; // Prevent double deletion
    other.hovered = false;
}

Button& Button::operator=(Button&& other) noexcept {
    if (this != &other) {
        if (texture) SDL_DestroyTexture(texture);
        rect = other.rect;
        label = std::move(other.label);
        font = other.font;
        renderer = other.renderer;
        texture = other.texture;
        onClick = std::move(other.onClick);
        hovered = other.hovered;
        other.texture = nullptr;
        other.hovered = false;
    }
    return *this;
}

void Button::render() {
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
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

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && inside) {
        onClick();
    }
}