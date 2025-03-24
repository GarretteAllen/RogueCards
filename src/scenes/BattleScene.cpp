#include "BattleScene.h"
#include "../core/Game.h"
#include <iostream>
#include <algorithm> 
#include <random>    

BattleScene::BattleScene(SDL_Renderer* renderer, TTF_Font* font, const Enemy& e, Game* game)
    : renderer(renderer), font(font), game(game), enemy(e), enemyHPText(nullptr),
    playerHP(20), playerHPText(nullptr), playerArmor(0), armorText(nullptr),
    battleWon(false), playerDefeated(false),
    continueButton(350, 400, 100, 50, "Continue", font, renderer, [this]() { this->game->endBattle(true); }),
    boardRect{ 300, 150, 200, 200 }, playerEnergy(3), maxEnergy(3), energyText(nullptr),
    skipTurnButton(350, 500, 100, 50, "Skip Turn", font, renderer, [this]() { this->endTurn(); }) {
    hand.reserve(5);
    drawPile.reserve(7); 

    const std::vector<Card>& selectedDeck = game->getSelectedDeck();
    std::cout << "Selected deck size: " << selectedDeck.size() << "\n";
    for (const Card& card : selectedDeck) {
 
        Card newCard(0, 0, card.getName(), card.getDamage(), card.getEnergyCost(), renderer, font, card.getEffect());
        drawPile.push_back(std::move(newCard));
    }
    std::cout << "drawPile size after population: " << drawPile.size() << "\n";

    updateHPText();
    updatePlayerHPText();
    updateArmorText();
    updateEnergyText();
    for (int i = 0; i < 3 && !drawPile.empty(); ++i) {
        drawCard();
    }
}

BattleScene::~BattleScene() {
    if (enemyHPText) { SDL_DestroyTexture(enemyHPText); enemyHPText = nullptr; }
    if (playerHPText) { SDL_DestroyTexture(playerHPText); playerHPText = nullptr; }
    if (armorText) { SDL_DestroyTexture(armorText); armorText = nullptr; }
    if (energyText) { SDL_DestroyTexture(energyText); energyText = nullptr; }
}
void BattleScene::initializeCards() { // moved functionality to deckselectionscene.cpp
}

void BattleScene::update() {
    if (playerDefeated) {
        battleWon = false;
        game->endBattle(false);
    }
}

void BattleScene::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &boardRect);

    for (size_t i = 0; i < hand.size(); ++i) {
        if (!hand[i].isDragging()) {
            int newX = 50 + i * 110;
            hand[i].getRect() = { newX, 450, 100, 150 };
            hand[i].getOriginalRect() = { newX, 450, 100, 150 };
        }
        hand[i].render(renderer);
    }

    if (!discard.empty()) {
        SDL_Rect discardRect = { 600, 450, 100, 150 };
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(renderer, &discardRect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &discardRect);
    }

    if (enemyHPText) SDL_RenderCopy(renderer, enemyHPText, nullptr, &enemyHPRect);
    if (playerHPText) SDL_RenderCopy(renderer, playerHPText, nullptr, &playerHPRect);
    if (armorText) SDL_RenderCopy(renderer, armorText, nullptr, &armorRect);
    if (energyText) SDL_RenderCopy(renderer, energyText, nullptr, &energyRect);

    if (battleWon || playerDefeated) {
        continueButton.render(renderer);
    }
    else {
        skipTurnButton.render(renderer);
    }
}
void BattleScene::handleInput(SDL_Event* event) {
    if (battleWon) {
        continueButton.handleEvent(event);
    }
    else {
        for (auto it = hand.begin(); it != hand.end(); ) {
            bool wasDragging = it->isDragging();
            it->handleEvent(event);
            if (event->type == SDL_MOUSEBUTTONUP && wasDragging && !it->isDragging()) {
                size_t index = std::distance(hand.begin(), it);
                playCardFromHand(*it, it);
                if (index >= hand.size()) {
                    break;
                }
                it = hand.begin() + index;
                if (it == hand.end()) break;
            }
            ++it;
        }
        skipTurnButton.handleEvent(event);
    }
}

void BattleScene::updateHPText() {
    if (enemyHPText) SDL_DestroyTexture(enemyHPText);
    enemyHPText = nullptr;
    std::string hpText = enemy.name + " HP: " + std::to_string(enemy.hp); // Include name
    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, hpText.c_str(), textColor);
    if (!surface) {
        std::cerr << "Failed to create enemy HP text surface: " << TTF_GetError() << std::endl;
        return;
    }
    enemyHPText = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (enemyHPText) {
        int w, h;
        SDL_QueryTexture(enemyHPText, nullptr, nullptr, &w, &h);
        enemyHPRect = { 10, 10, w, h };
    }
}

void BattleScene::updatePlayerHPText() {
    if (playerHPText) SDL_DestroyTexture(playerHPText);
    playerHPText = nullptr;
    std::string hpText = "Player HP: " + std::to_string(playerHP);
    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, hpText.c_str(), textColor);
    if (!surface) {
        std::cerr << "Failed to create player HP text surface: " << TTF_GetError() << std::endl;
        return;
    }
    playerHPText = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (playerHPText) {
        int w, h;
        SDL_QueryTexture(playerHPText, nullptr, nullptr, &w, &h);
        playerHPRect = { 10, 70, w, h }; 
    }
}

void BattleScene::updateEnergyText() {
    if (energyText) SDL_DestroyTexture(energyText);
    energyText = nullptr;
    std::string energyStr = "Energy: " + std::to_string(playerEnergy) + "/" + std::to_string(maxEnergy);
    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, energyStr.c_str(), textColor);
    if (!surface) {
        std::cerr << "Failed to create energy text surface: " << TTF_GetError() << std::endl;
        return;
    }
    energyText = SDL_CreateTextureFromSurface(renderer, surface);
    std::cout << "Energy text created at " << energyText << "\n";
    SDL_FreeSurface(surface);
    if (energyText) {
        int w, h;
        SDL_QueryTexture(energyText, nullptr, nullptr, &w, &h);
        energyRect = { 10, 40, w, h };
    }
}

void BattleScene::playCard(Card& card) {
    enemy.hp -= card.getDamage();
    std::cout << "Played " << card.getName() << ", " << enemy.name << " HP now: " << enemy.hp << std::endl;
    updateHPText();

    CardEffect effect = card.getEffect();
    switch (effect.type) {
    case CardEffectType::Armor:
        playerArmor += effect.value;
        updateArmorText();
        break;
    case CardEffectType::Heal:
        playerHP = std::min(20, playerHP + effect.value);
        updatePlayerHPText();
        break;
    case CardEffectType::MultiStrike:
        for (int i = 0; i < effect.count; ++i) {
            enemy.hp -= effect.value;
            updateHPText();
            if (enemy.hp <= 0) {
                battleWon = true;
                break;
            }
        }
        break;
    case CardEffectType::Weaken:
        applyWeakenEffect(effect.value, effect.count);
        break;
    case CardEffectType::Poison:
        applyPoisonEffect(effect.value, effect.count);
        break;
    case CardEffectType::Thorns:
        applyThornsEffect();
        break;
    default:
        break;
    }

    discard.push_back(std::move(card));
    if (enemy.hp <= 0) battleWon = true;
}

void BattleScene::playCardFromHand(Card& card, std::vector<Card>::iterator& it) {
    SDL_Rect cardRect = card.getRect();
    std::cout << "Dropped " << card.getName() << " at (" << cardRect.x << ", " << cardRect.y << ")" << std::endl;
    std::cout << "Board rect: (" << boardRect.x << ", " << boardRect.y << ", " << boardRect.w << ", " << boardRect.h << ")" << std::endl;
    if (SDL_HasIntersection(&cardRect, &boardRect)) {
        if (playerEnergy >= card.getEnergyCost()) {
            std::cout << card.getName() << " intersects board, playing card (Cost: " << card.getEnergyCost() << ")" << std::endl;
            playerEnergy -= card.getEnergyCost();
            updateEnergyText();
            playCard(card);
            it = hand.erase(it);
            endTurn(); 
        }
        else {
            std::cout << "Not enough energy (" << playerEnergy << ") to play " << card.getName() << " (Cost: " << card.getEnergyCost() << ")" << std::endl;
            card.resetPosition();
            ++it;
        }
    }
    else {
        std::cout << card.getName() << " dropped outside board, resetting to (" << card.getOriginalRect().x << ", " << card.getOriginalRect().y << ")" << std::endl;
        card.resetPosition();
        ++it;
    }
}

void BattleScene::drawCard() {
    if (drawPile.empty() && !discard.empty()) {
        std::cout << "Before move, discard top: " << discard.back().getName()
            << " effect type: " << (int)discard.back().getEffect().type
            << ", value: " << discard.back().getEffect().value << std::endl;
        drawPile = std::move(discard);
        discard.clear();
        std::cout << "After move, drawPile top: " << drawPile.back().getName()
            << " effect type: " << (int)drawPile.back().getEffect().type
            << ", value: " << drawPile.back().getEffect().value << std::endl;
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(drawPile.begin(), drawPile.end(), g);
        std::cout << "After shuffle, drawPile top: " << drawPile.back().getName()
            << " effect type: " << (int)drawPile.back().getEffect().type
            << ", value: " << drawPile.back().getEffect().value << std::endl;
    }
    if (!drawPile.empty() && hand.size() < 5) {
        Card card = std::move(drawPile.back());
        drawPile.pop_back();
        int newX = 50 + hand.size() * 110;
        SDL_Rect newRect = { newX, 450, 100, 150 };
        card.getRect() = newRect;
        card.getOriginalRect() = newRect;
        std::cout << "Moving " << card.getName() << " with effect type: " << (int)card.getEffect().type
            << ", value: " << card.getEffect().value << std::endl;
        hand.push_back(std::move(card));
        std::cout << "Drew " << hand.back().getName() << " at (" << hand.back().getRect().x << ", " << hand.back().getRect().y
            << "), effect type: " << (int)hand.back().getEffect().type << ", value: " << hand.back().getEffect().value
            << ", hand size: " << hand.size() << std::endl;
    }
}

void BattleScene::enemyAttack() {
    int effectiveDamage = std::max(0, enemy.damage - enemy.damageReduction);
    int damageAfterArmor = std::max(0, effectiveDamage - playerArmor);
    playerArmor = std::max(0, playerArmor - effectiveDamage);
    playerHP -= damageAfterArmor;
    std::cout << enemy.name << " attacks for " << effectiveDamage << " damage (base: " << enemy.damage
        << ", reduced by " << enemy.damageReduction << "), reduced by "
        << (effectiveDamage - damageAfterArmor) << " armor, player HP now: " << playerHP
        << ", armor now: " << playerArmor << std::endl;
    updatePlayerHPText();
    updateArmorText();
    if (playerHP <= 0) {
        std::cout << "Player defeated!" << std::endl;
        playerDefeated = true;
    }
}

void BattleScene::updateArmorText() {
    if (armorText) SDL_DestroyTexture(armorText);
    armorText = nullptr;
    std::string armorStr = "Armor: " + std::to_string(playerArmor);
    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, armorStr.c_str(), textColor);
    if (!surface) {
        std::cerr << "Failed to create armor text surface: " << TTF_GetError() << std::endl;
        return;
    }
    armorText = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (armorText) {
        int w, h;
        SDL_QueryTexture(armorText, nullptr, nullptr, &w, &h);
        armorRect = { 10, 100, w, h }; // Below HP
    }
}

void BattleScene::endTurn() {
    enemyAttack();
    updateEnemyEffects();
    drawCard();
    resetTurn();
}

void BattleScene::resetTurn() {
    playerEnergy = maxEnergy;
    updateEnergyText();
    std::cout << "Turn reset, energy restored to " << playerEnergy << std::endl;
}

void BattleScene::applyWeakenEffect(int reduction, int turns) {
    enemy.damageReduction = reduction;
    enemy.weakenTurns = turns;
    std::cout << enemy.name << " weakened: damage reduced by " << reduction << " for " << turns << " turns\n";
}

void BattleScene::applyPoisonEffect(int damage, int turns) {
    enemy.poisonDamage = damage;
    enemy.poisonTurns = turns;
    std::cout << enemy.name << " poisoned: " << damage << " dmg/turn for " << turns << " turns\n";
}

void BattleScene::applyThornsEffect() {
    enemy.hp -= playerArmor;
    std::cout << "Thorns dealt " << playerArmor << " damage, " << enemy.name << " HP now: " << enemy.hp << "\n";
    updateHPText();
}

void BattleScene::updateEnemyEffects() {
    if (enemy.weakenTurns > 0) {
        enemy.weakenTurns--;
        if (enemy.weakenTurns == 0) enemy.damageReduction = 0;
    }
    if (enemy.poisonTurns > 0) {
        enemy.hp -= enemy.poisonDamage;
        enemy.poisonTurns--;
        std::cout << enemy.name << " takes " << enemy.poisonDamage << " poison damage, HP now: " << enemy.hp
            << ", poison turns left: " << enemy.poisonTurns << "\n";
        updateHPText();
        if (enemy.hp <= 0) battleWon = true;
    }
}
