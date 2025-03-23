#include "Button.h"
#include <iostream>

Button::Button(int x, int y, int w, int h, const std::string& label, TTF_Font* font, SDL_Renderer* renderer, std::function<void()> onClick)
    : rect{ x, y, w, h }, label(label), font(font), renderer(renderer), onClick(onClick), hovered(false), texture(nullptr) {
    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, label.c_str(), textColor);
    if (surface) {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        std::cout << "Button '" << label << "' texture created successfully\n";
        SDL_FreeSurface(surface);
    }
    else {
        std::cerr << "Failed to create button texture for '" << label << "': " << TTF_GetError() << std::endl;
    }
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

void Button::render(SDL_Renderer* renderer) {
    SDL_Color color = hovered ? SDL_Color{ 200, 200, 200, 255 } : SDL_Color{ 150, 150, 150, 255 };
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
    if (texture) {
        int texW, texH;
        if (SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH) == 0) {
            SDL_Rect dst = { rect.x + (rect.w - texW) / 2, rect.y + (rect.h - texH) / 2, texW, texH };
            SDL_RenderCopy(renderer, texture, nullptr, &dst);
        }
        else {
            std::cerr << "Failed to query texture for '" << label << "': " << SDL_GetError() << "\n";
        }
    }
    else {
        std::cerr << "No texture for button '" << label << "'\n";
    }
}

void Button::handleEvent(SDL_Event* event) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    bool inside = (mouseX >= rect.x && mouseX <= rect.x + rect.w && mouseY >= rect.y && mouseY <= rect.y + rect.h);

    switch (event->type) {
    case SDL_MOUSEMOTION:
        hovered = inside;
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (inside && event->button.button == SDL_BUTTON_LEFT) {
            onClick();
        }
        break;
    }
}