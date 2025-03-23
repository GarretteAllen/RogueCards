#ifndef NODE_H
#define NODE_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <functional>

class Node {
public:
    Node(int x, int y, int size, const std::string& label, float opacity,
        SDL_Renderer* renderer, TTF_Font* font, std::function<void()> onClick);
    ~Node();
    void render(SDL_Renderer* renderer);
    void handleEvent(SDL_Event* event);

    std::string label;
    float opacity;
    bool isHovered;
    bool isCompleted;

private:
    SDL_Rect rect;
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Texture* texture;
    std::function<void()> onClick;
};

#endif