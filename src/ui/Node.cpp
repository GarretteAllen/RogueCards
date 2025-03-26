#include "../includes/ui/Node.h"
#include <iostream>

Node::Node(int x, int y, int size, const std::string& label, float opacity,
    SDL_Renderer* renderer, TTF_Font* font,
    std::function<void()> onClick, NodeType type,
    SDL_Color color, std::vector<int> nextNodes)
    : rect{ x, y, size, size }, label(label), opacity(opacity), isCompleted(false),
    renderer(renderer), font(font), onClick(onClick), type(type),
    color(color), nextNodes(nextNodes), labelTexture(nullptr), needsTextureUpdate(true) {
    // Defer label creation to render()
}

Node::~Node() {
    if (labelTexture) {
        SDL_DestroyTexture(labelTexture);
        labelTexture = nullptr;
    }
}

void Node::setRenderer(SDL_Renderer* newRenderer) {
    renderer = newRenderer;
    if (labelTexture) {
        SDL_DestroyTexture(labelTexture);
        labelTexture = nullptr;
    }
    needsTextureUpdate = true; // Defer texture creation to render()
}

void Node::setFont(TTF_Font* newFont) {
    font = newFont;
    if (labelTexture) {
        SDL_DestroyTexture(labelTexture);
        labelTexture = nullptr;
    }
    needsTextureUpdate = true; // Defer texture creation to render()
}

void Node::createLabelTexture() {
    if (labelTexture) {
        SDL_DestroyTexture(labelTexture);
        labelTexture = nullptr;
    }

    if (!font) {
        std::cerr << "Font is null for label: " << label << std::endl;
        return;
    }

    if (!renderer) {
        std::cerr << "Renderer is null for label: " << label << std::endl;
        return;
    }

    SDL_Color textColor = { 0, 0, 0, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, label.c_str(), textColor);
    if (!surface) {
        std::cerr << "Failed to create surface for label '" << label << "': " << TTF_GetError() << std::endl;
        return;
    }

    labelTexture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!labelTexture) {
        std::cerr << "Failed to create texture for label '" << label << "': " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }

    labelRect.w = surface->w;
    labelRect.h = surface->h;
    labelRect.x = rect.x + (rect.w - labelRect.w) / 2;
    labelRect.y = rect.y + rect.h + 5;

    std::cout << "Created label texture for '" << label << "' at (" << labelRect.x << ", " << labelRect.y << ")" << std::endl;

    SDL_FreeSurface(surface);
    needsTextureUpdate = false;
}

bool Node::handleEvent(SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int x = e.button.x;
        int y = e.button.y;
        if (x >= rect.x && x <= rect.x + rect.w &&
            y >= rect.y && y <= rect.y + rect.h) {
            if (onClick) {
                onClick();
                return true;
            }
        }
    }
    return false;
}

void Node::render() const {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, static_cast<Uint8>(opacity * 255));
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(renderer, &rect);

    if (opacity < 1.0f && !isCompleted) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);
    }

    // Create label texture if needed (non-const cast to modify labelTexture)
    Node* nonConstThis = const_cast<Node*>(this);
    if (nonConstThis->needsTextureUpdate && nonConstThis->renderer && nonConstThis->font) {
        nonConstThis->createLabelTexture();
    }

    if (labelTexture) {
        SDL_Rect labelBgRect = labelRect;
        labelBgRect.x -= 2;
        labelBgRect.y -= 2;
        labelBgRect.w += 4;
        labelBgRect.h += 4;
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 200);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(renderer, &labelBgRect);

        SDL_SetTextureAlphaMod(labelTexture, static_cast<Uint8>(opacity * 255));
        SDL_RenderCopy(renderer, labelTexture, nullptr, &labelRect);
    }
    else {
        std::cerr << "No label texture for '" << label << "' during render" << std::endl;
    }
}