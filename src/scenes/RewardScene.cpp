#include "../includes/scenes/RewardScene.h"
#include "../includes/common/Constants.h"
#include "../includes/core/Game.h"
#include "../includes/scenes/GameScene.h"
#include <random>
#include <iostream>

RewardScene::RewardScene(SDL_Renderer* renderer, TTF_Font* font, Game* game, RewardType rewardType)
    : renderer(renderer), font(font), game(game), rewardType(rewardType), skipButtonTexture(nullptr) {
    initializeRewardCards();
    createSkipButton();
}

RewardScene::~RewardScene() {
    if (skipButtonTexture) {
        SDL_DestroyTexture(skipButtonTexture);
    }
}

void RewardScene::setRenderer(SDL_Renderer* newRenderer) {
    renderer = newRenderer;
    if (skipButtonTexture) {
        SDL_DestroyTexture(skipButtonTexture);
        skipButtonTexture = nullptr;
    }
    createSkipButton();
}

void RewardScene::setFont(TTF_Font* newFont) {
    font = newFont;
    for (auto& card : rewardCards) {
        card.setFont(font);
    }
}

void RewardScene::initializeRewardCards() {
    rewardCards.clear();
    cardRects.clear();

    std::random_device rd;
    std::mt19937 gen(rd());

    for (int i = 0; i < 3; ++i) {
        Game::CardRarity maxRarity = (rewardType == RewardType::Green) ? Game::CardRarity::Rare : Game::CardRarity::Epic;
        Game::CardRarity rarity = Game::CardRarity::Common;
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        float roll = dist(gen);

        if (rewardType == RewardType::Green) {
            if (roll < Constants::RARITY_PROBABILITY_THRESHOLD) {
                rarity = Game::CardRarity::Rare;
            }
        }
        else if (rewardType == RewardType::Purple) {
            if (roll < Constants::RARITY_PROBABILITY_THRESHOLD) {
                rarity = Game::CardRarity::Epic;
            }
            else {
                roll = dist(gen);
                if (roll < Constants::RARITY_PROBABILITY_THRESHOLD) {
                    rarity = Game::CardRarity::Rare;
                }
            }
        }

        std::vector<Card> possibleCards = game->getRewardCards(maxRarity, 1);
        if (!possibleCards.empty()) {
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
                Card card = filteredCards[0];
                card.setPosition(Constants::REWARD_CARD_BASE_X + i * Constants::REWARD_CARD_SPACING, Constants::REWARD_CARD_Y);
                rewardCards.push_back(card);

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
    skipButtonRect.y = game->getWindowHeight() - 150; // Adjust position dynamically

    SDL_FreeSurface(surface);
}

void RewardScene::render() {
    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    SDL_RenderClear(renderer);

    for (size_t i = 0; i < rewardCards.size(); ++i) {
        Card& card = rewardCards[i];
        card.render(renderer, 999, game->getWindowWidth(), game->getWindowHeight());

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &cardRects[i]);
    }

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

        for (size_t i = 0; i < cardRects.size(); ++i) {
            const SDL_Rect& rect = cardRects[i];
            if (x >= rect.x && x <= rect.x + rect.w &&
                y >= rect.y && y <= rect.y + rect.h) {
                game->addCardToDeck(rewardCards[i]);
                game->setState(Game::GameState::GAME);
                return;
            }
        }

        if (x >= skipButtonRect.x && x <= skipButtonRect.x + skipButtonRect.w &&
            y >= skipButtonRect.y && y <= skipButtonRect.y + skipButtonRect.h) {
            game->setState(Game::GameState::GAME);
            return;
        }
    }
}