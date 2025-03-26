#ifndef BATTLE_SCENE_H
#define BATTLE_SCENE_H

#include "Scene.h"
#include "../ui/Card.h"
#include "../ui/Button.h"
#include "../entities/Enemy.h"
#include "../systems/TextureManager.h"
#include <vector>
#include <functional>

class Game;

class BattleScene : public Scene {
public:
    BattleScene(SDL_Renderer* renderer, TTF_Font* font, const Enemy& enemy, Game* game);
    void render() override;
    void handleEvent(SDL_Event& e) override;
    bool isBattleOver() const;
    bool hasPlayerWon() const;
    bool isReadyToEnd() const; 
    void setRenderer(SDL_Renderer* renderer) override;
    void setFont(TTF_Font* font) override;

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    Game* game;
    Enemy enemy;
    SDL_Texture* enemyHPText;
    int playerHP;
    SDL_Texture* playerHPText;
    int playerArmor;
    SDL_Texture* armorText;
    bool battleWon;
    bool playerDefeated;
    bool readyToEnd; 
    Button continueButton;
    SDL_Rect boardRect;
    std::vector<Card> hand;
    std::vector<Card> drawPile;
    std::vector<Card> discard;
    int playerEnergy;
    int maxEnergy;
    SDL_Texture* energyText;
    Button skipTurnButton;
    TextureManager textureManager;
    SDL_Rect playerTextRect;
    SDL_Texture* playerText;
    SDL_Rect enemyTextRect;
    SDL_Texture* enemyText;


    void updateHPText();
    void updatePlayerHPText();
    void updateArmorText();
    void updateEnergyText();
    void drawCard();
    void playCard(Card& card);
    void applyWeakenEffect(int value, int turns);
    void applyPoisonEffect(int damage, int turns);
    void applyThornsEffect();
    void applyWetEffect(int turns);
    void applyLightningEffect(int damage);
    void applyIceEffect(int damage, int freezeTurns);
    void updateEnemyEffects();
    void enemyAttack();
    void endTurn();
    void resetTurn();
    void updateCardPositions();
    void updateTextTextures();
};

#endif