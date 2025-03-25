#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SDL.h>
#include <string>

namespace Constants {
    // Window dimensions
    inline constexpr int WINDOW_WIDTH = 800;
    inline constexpr int WINDOW_HEIGHT = 600;

    // Card dimensions
    inline constexpr int CARD_WIDTH = 150;
    inline constexpr int CARD_HEIGHT = 200;
    inline constexpr Uint32 CARD_HOVER_DELAY = 2000;
    inline constexpr float CARD_MAGNIFICATION_SCALE = 1.5f;
    inline constexpr Uint8 CARD_LOW_ENERGY_ALPHA = 128;
    inline constexpr Uint8 CARD_FULL_ALPHA = 255;

    // Reward scene card positions
    inline constexpr int REWARD_CARD_BASE_X = 200;
    inline constexpr int REWARD_CARD_Y = 200;
    inline constexpr int REWARD_CARD_SPACING = 150;

    // Reward scene rarity probabilities
    inline constexpr float RARITY_PROBABILITY_THRESHOLD = 0.5f;

    // File paths
    inline const std::string ASSET_PATH = "assets/";
    inline const std::string CARD_PATH = ASSET_PATH + "cards/";
    inline const std::string CARD_SUFFIX = "_card.png";
    inline const std::string FONT_PATH = ASSET_PATH + "fonts/arial.ttf";
    inline constexpr int FONT_SIZE = 24;

    // Colors
    inline const SDL_Color COLOR_WHITE = { 255, 255, 255, 255 };
    inline const SDL_Color COLOR_GRAY = { 200, 200, 200, 255 };
}

#endif