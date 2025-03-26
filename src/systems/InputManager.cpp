#include "../includes/systems/InputManager.h"

void InputManager::processInput(bool& isRunning) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			isRunning = false;
		}
	}
}