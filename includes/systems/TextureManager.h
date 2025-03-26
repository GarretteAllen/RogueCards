#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <map>
#include <memory>
#include <iostream>

class TextureManager {
public:
	TextureManager() = default;
	~TextureManager() = default;

	std::shared_ptr<SDL_Texture> loadTexture(const std::string& path, SDL_Renderer* renderer) {
		auto it = textures.find(path);
		if (it != textures.end()) {
			return it->second;
		}

		SDL_Surface* surface = IMG_Load(path.c_str());
		if (!surface) {
			std::cerr << "Failed to load image from " << path << " - IMG_Error: " << IMG_GetError() << std::endl;
			return nullptr;
		}

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);

		if (!texture) {
			std::cerr << "Failed to create texture from " << path << " - SDL_Error: " << SDL_GetError() << std::endl;
			return nullptr;
		}

		auto sharedTexture = std::shared_ptr<SDL_Texture>(texture, SDL_DestroyTexture);
		textures[path] = sharedTexture;
		std::cout << "Loaded texture from " << path << std::endl;
		return sharedTexture;
	}

	void clear() {
		textures.clear();
	}
private:
	std::map<std::string, std::shared_ptr<SDL_Texture>> textures;
};

#endif