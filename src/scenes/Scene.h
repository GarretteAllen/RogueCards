#ifndef SCENE_H
#define SCENE_H

#include <SDL.h>

class Scene {
public:
    virtual ~Scene() = default;
    virtual void render() = 0;
    virtual void handleEvent(SDL_Event& e) = 0; 
};

#endif