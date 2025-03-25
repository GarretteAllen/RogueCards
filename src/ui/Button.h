#ifndef BUTTON_H
#define BUTTON_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <functional>

class Button {
public:
    Button(int x, int y, int w, int h, const std::string& label, TTF_Font* font, SDL_Renderer* renderer,
        std::function<void()> onClick);
    ~Button();
    Button(Button&& other) noexcept; 
    Button& operator=(Button&& other) noexcept; 
    Button(const Button&) = delete; 
    Button& operator=(const Button&) = delete; 
    void render();
    void handleEvent(SDL_Event& e); 
    SDL_Rect getRect() const { return rect; }
    SDL_Texture* getTexture() const { return texture; }

private:
    SDL_Rect rect;
    std::string label;
    TTF_Font* font;
    SDL_Renderer* renderer; 
    SDL_Texture* texture;
    std::function<void()> onClick;
    bool hovered;
};

#endif