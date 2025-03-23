#include "Node.h"
#include <iostream>

Node::Node(int x, int y, int size, const std::string& label, float opacity,
    SDL_Renderer* renderer, TTF_Font* font, std::function<void()> onClick)
    : rect{ x, y, size, size }, label(label), opacity(opacity), renderer(renderer),
    font(font), onClick(onClick), isHovered(false), isCompleted(false), texture(nullptr) {
    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, label.c_str(), textColor);
    if (surface) {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        std::cout << "Node '" << label << "' texture created successfully\n";
        SDL_FreeSurface(surface);
    }
    else {
        std::cerr << "Failed to create node texture for '" << label << "': " << TTF_GetError() << std::endl;
    }
}

Node::~Node() {
    if (texture) SDL_DestroyTexture(texture);
}

void Node::render(SDL_Renderer* renderer) {
    SDL_Color color;
    if (isCompleted) {
        color = { 100, 100, 100, 255 }; // Dim gray for completed
    }
    else if (opacity >= 1.0f) {
        color = { 0, 255, 0, 255 }; // Green for unlocked
    }
    else {
        color = { 150, 150, 150, 255 }; // Gray for locked
    }
    if (isHovered && opacity >= 1.0f && !isCompleted) {
        color = { 0, 200, 0, 255 }; // Darker green on hover
    }
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &rect);

    if (texture) {
        int texW, texH;
        SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);
        SDL_Rect dst = { rect.x + (rect.w - texW) / 2, rect.y + (rect.h - texH) / 2, texW, texH };
        SDL_SetTextureAlphaMod(texture, static_cast<Uint8>(opacity * 255));
        SDL_RenderCopy(renderer, texture, nullptr, &dst);
    }
}

void Node::handleEvent(SDL_Event* event) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    bool inside = (mouseX >= rect.x && mouseX <= rect.x + rect.w &&
        mouseY >= rect.y && mouseY <= rect.y + rect.h);

    if (opacity < 1.0f || isCompleted) return; // Ignore clicks on locked or completed nodes

    switch (event->type) {
    case SDL_MOUSEMOTION:
        isHovered = inside;
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (inside && event->button.button == SDL_BUTTON_LEFT) {
            onClick();
        }
        break;
    }
}