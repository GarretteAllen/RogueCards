#ifndef SCENE_H
#define SCENE_H

#include <SDL.h>

class Scene {
public:
	virtual ~Scene() {}
	virtual void update() = 0;
	virtual void render(SDL_Renderer* renderer) = 0;
	virtual void handleInput(SDL_Event* event) = 0;
};

#endif