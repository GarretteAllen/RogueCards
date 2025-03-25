#ifndef REWARD_SCENE_H
#define REWARD_SCENE_H

#include "Scene.h"
#include "../ui/Card.h"
#include <vector>

class Game;

class RewardScene : public Scene {
public:
    enum class RewardType { Green, Purple };

    RewardScene(SDL_Renderer* renderer, TTF_Font* font, Game* game, RewardType rewardType);
    void render() override;
    void handleEvent(SDL_Event& e) override;

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    Game* game;
    RewardType rewardType;
    std::vector<Card> rewardCards;
    std::vector<SDL_Rect> cardRects;
    SDL_Rect skipButtonRect;
    SDL_Texture* skipButtonTexture;

    void initializeRewardCards();
    void createSkipButton();
};

#endif