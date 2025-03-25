#include "RewardScene.h"
#include "../common/Constants.h"
#include "../core/Game.h"
#include "../scenes/GameScene.h"
#include <random>
#include <iostream>

RewardScene::RewardScene(SDL_Renderer* renderer, TTF_Font* font, Game* game, RewardType rewardType)
    : renderer(renderer), font(font), game(game), rewardType(rewardType) {
    initializeRewardCards();
    createSkipButton();
}

void RewardScene::initializeRewardCards() {
    rewardCards.clear();
    cardRects.clear();

    // Get three random cards based on the reward type
    std::random_device rd;
    std::mt19937 gen(rd());

    for (int i = 0; i < 3; ++i) {
        // Determine the rarity for this card
        Game::CardRarity maxRarity = (rewardType == RewardType::Green) ? Game::CardRarity::Rare : Game::CardRarity::Epic;
        Game::CardRarity rarity = Game::CardRarity::Common;
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        float roll = dist(gen);

        if (rewardType == RewardType::Green) {
            // Green Reward: 50% chance for Rare, 50% chance for Common, no Epic
            if (roll < Constants::RARITY_PROBABILITY_THRESHOLD) {
                rarity = Game::CardRarity::Rare;
            }
        }
        else if (rewardType == RewardType::Purple) {
            // Purple Reward: 50% chance for Epic, 50% chance for Rare or Common
            if (roll < Constants::RARITY_PROBABILITY_THRESHOLD) {
                rarity = Game::CardRarity::Epic;
            }
            else {
                // If not Epic, 50% chance for Rare, 50% chance for Common
                roll = dist(gen);
                if (roll < Constants::RARITY_PROBABILITY_THRESHOLD) {
                    rarity = Game::CardRarity::Rare;
                }
            }
        }

        // Get a random card of the determined rarity
        std::vector<Card> possibleCards = game->getRewardCards(maxRarity, 1);
        if (!possibleCards.empty()) {
            // Filter cards by the exact rarity we want
            std::vector<Card> filteredCards;
            for (const auto& card : possibleCards) {
                Game::CardRarity cardRarity = Game::CardRarity::Common;
                std::string name = card.getName();
                if (name == "Lightning Strike" || name == "Ice Shard" || name == "Superb Shield") {
                    cardRarity = Game::CardRarity::Rare;
                }
                else if (name == "Dragon's Breath") {
                    cardRarity = Game::CardRarity::Epic;
                }
                if (cardRarity == rarity) {
                    filteredCards.push_back(card);
                }
            }

            if (filteredCards.empty()) {
                // Fallback to Common if no cards of the desired rarity are found
                for (const auto& card : possibleCards) {
                    Game::CardRarity cardRarity = Game::CardRarity::Common;
                    std::string name = card.getName();
                    if (name == "Lightning Strike" || name == "Ice Shard" || name == "Superb Shield") {
                        cardRarity = Game::CardRarity::Rare;
                    }
                    else if (name == "Dragon's Breath") {
                        cardRarity = Game::CardRarity::Epic;
                    }
                    if (cardRarity == Game::CardRarity::Common) {
                        filteredCards.push_back(card);
                    }
                }
            }

            if (!filteredCards.empty()) {
                Card card = filteredCards[0]; // Pick the first card
                card.setPosition(Constants::REWARD_CARD_BASE_X + i * Constants::REWARD_CARD_SPACING, Constants::REWARD_CARD_Y);
                rewardCards.push_back(card);

                // Set up clickable area for the card
                SDL_Rect cardRect = { Constants::REWARD_CARD_BASE_X + i * Constants::REWARD_CARD_SPACING, Constants::REWARD_CARD_Y, Constants::CARD_WIDTH, Constants::CARD_HEIGHT };
                cardRects.push_back(cardRect);
            }
        }
    }
}

void RewardScene::createSkipButton() {
    SDL_Color textColor = { 0, 0, 0, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, "Skip", textColor);
    if (!surface) {
        std::cerr << "Failed to create surface for Skip button: " << TTF_GetError() << std::endl;
        return;
    }

    skipButtonTexture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!skipButtonTexture) {
        std::cerr << "Failed to create texture for Skip button: " << SDL_GetError() << std::endl;
    }

    skipButtonRect.w = surface->w;
    skipButtonRect.h = surface->h;
    skipButtonRect.x = (game->getWindowWidth() - skipButtonRect.w) / 2;
    skipButtonRect.y = 450;

    SDL_FreeSurface(surface);
}

void RewardScene::render() {
    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    SDL_RenderClear(renderer);

    // Render the cards
    for (size_t i = 0; i < rewardCards.size(); ++i) {
        Card& card = rewardCards[i]; // Remove const to allow calling non-const render
        // Pass default values for playerEnergy, windowWidth, and windowHeight
        // playerEnergy is set high so cards are not dimmed
        card.render(renderer, 999, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);

        // Draw a border around the card
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &cardRects[i]);
    }

    // Render the Skip button
    if (skipButtonTexture) {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderFillRect(renderer, &skipButtonRect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &skipButtonRect);
        SDL_RenderCopy(renderer, skipButtonTexture, nullptr, &skipButtonRect);
    }

    SDL_RenderPresent(renderer);
}

void RewardScene::handleEvent(SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int x = e.button.x;
        int y = e.button.y;

        // Check if a card was clicked
        for (size_t i = 0; i < cardRects.size(); ++i) {
            const SDL_Rect& rect = cardRects[i];
            if (x >= rect.x && x <= rect.x + rect.w &&
                y >= rect.y && y <= rect.y + rect.h) {
                game->addCardToDeck(rewardCards[i]);
                game->setState(Game::GameState::GAME);
                // Move updateProgression to Game::setState to avoid accessing this after destruction
                return;
            }
        }

        // Check if the Skip button was clicked
        if (x >= skipButtonRect.x && x <= skipButtonRect.x + skipButtonRect.w &&
            y >= skipButtonRect.y && y <= skipButtonRect.y + skipButtonRect.h) {
            game->setState(Game::GameState::GAME);
            // Move updateProgression to Game::setState to avoid accessing this after destruction
            return;
        }
    }
}