#ifndef SCENE_H
#define SCENE_H

#include <SDL.h>
#include <SDL_ttf.h>

class Scene {
public:
    virtual ~Scene() = default;
    virtual void render() = 0;
    virtual void handleEvent(SDL_Event& e) = 0;
    virtual void setRenderer(SDL_Renderer* renderer) = 0;
    virtual void setFont(TTF_Font* font) = 0; // New method
};

#endif