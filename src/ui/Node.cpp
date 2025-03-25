#include "Node.h"
#include <iostream>

Node::Node(int x, int y, int size, const std::string& label, float opacity, SDL_Renderer* renderer, TTF_Font* font, std::function<void()> onClick)
    : rect{ x, y, size, size }, label(label), opacity(opacity), isCompleted(false), texture(nullptr), renderer(renderer), onClick(onClick) {
    SDL_Color textColor = { 0, 0, 0, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, label.c_str(), textColor);
    if (!surface) {
        std::cerr << "Failed to create surface for node: " << TTF_GetError() << std::endl;
        return;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

void Node::render() const {
    // Set color based on state
    if (isCompleted) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    }
    else if (opacity == 1.0f) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    }
    SDL_RenderFillRect(renderer, &rect);

    if (texture) {
        int texW, texH;
        SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);
        SDL_Rect textRect = { rect.x + (rect.w - texW) / 2, rect.y + (rect.h - texH) / 2, texW, texH };
        SDL_RenderCopy(renderer, texture, nullptr, &textRect);
    }
}

void Node::handleEvent(SDL_Event& e) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    bool inside = (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h);

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && inside && opacity == 1.0f && !isCompleted) {
        onClick();
    }
}