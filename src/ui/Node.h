#ifndef NODE_H
#define NODE_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <functional>

class Node {
public:
    Node(int x, int y, int size, const std::string& label, float opacity, SDL_Renderer* renderer, TTF_Font* font, std::function<void()> onClick);
    void render() const;
    void handleEvent(SDL_Event& e);

    std::string label; 
    float opacity; 
    bool isCompleted; 

private:
    SDL_Rect rect;
    SDL_Texture* texture;
    SDL_Renderer* renderer;
    std::function<void()> onClick;
};

#endif