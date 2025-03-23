#ifndef BUTTON_H
#define BUTTON_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <functional>

class Button {
public:
    Button(int x, int y, int w, int h, const std::string& label, TTF_Font* font, SDL_Renderer* renderer, std::function<void()> onClick);
    ~Button();
    Button(Button&& other) noexcept; // Move constructor
    Button& operator=(Button&& other) noexcept; // Move assignment
    Button(const Button&) = delete; // Disable copy
    Button& operator=(const Button&) = delete; // Disable copy assignment
    void render(SDL_Renderer* renderer);
    void handleEvent(SDL_Event* event);
    SDL_Rect getRect() const { return rect; }
    SDL_Texture* getTexture() const { return texture; }

private:
    SDL_Rect rect;
    std::string label;
    TTF_Font* font;
    SDL_Renderer* renderer; // Store for rendering
    SDL_Texture* texture;
    std::function<void()> onClick;
    bool hovered;
};

#endif