#ifndef BATTLESCENE_H
#define BATTLESCENE_H

#include "../entities/Enemy.h"
#include "Scene.h"
#include "../ui/Button.h"
#include "../ui/Card.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

class Game;

class BattleScene : public Scene {
public:
    BattleScene(SDL_Renderer* renderer, TTF_Font* font, const Enemy& enemy, Game* game);
    ~BattleScene();
    void update() override;
    void render(SDL_Renderer* renderer) override;
    void handleInput(SDL_Event* event) override;
    void playCardFromHand(Card& card, std::vector<Card>::iterator& it); 

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    Game* game;
    Enemy enemy;
    SDL_Texture* enemyHPText;
    SDL_Rect enemyHPRect;
    bool battleWon;
    Button continueButton;
    std::vector<Card> hand;
    std::vector<Card> discard;
    std::vector<Card> drawPile;
    SDL_Rect boardRect;
    //player stats / rects
    int playerHP;
    SDL_Texture* playerHPText;
	SDL_Rect playerHPRect;
    int playerEnergy;        
    int maxEnergy;           
    SDL_Texture* energyText; 
    SDL_Rect energyRect;  
    Button skipTurnButton;
    bool playerDefeated = false;
    int playerArmor;         
    SDL_Texture* armorText;
    SDL_Rect armorRect;

    void updateHPText();    
    void updateArmorText();
    void updatePlayerHPText();
    void updateEnergyText(); 
    void initializeCards();  
    void playCard(Card& card); 
    void drawCard();
    void enemyAttack();
    void endTurn();
    void resetTurn();     
    void applyWeakenEffect(int reduction, int turns);
    void updateEnemyEffects();
    void applyPoisonEffect(int damage, int turns);
    void applyThornsEffect();
};

#endif