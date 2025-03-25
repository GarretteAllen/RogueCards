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
        std::string imagePath = "assets/cards/" + card.getName() + "_card.png";
        std::replace(imagePath.begin(), imagePath.end(), ' ', '_');
        std::transform(imagePath.begin(), imagePath.end(), imagePath.begin(), ::tolower);
        newCard.loadImage(imagePath, renderer);
        drawPile.push_back(std::move(newCard));
    }
    std::cout << "drawPile size after population: " << drawPile.size() << "\n";

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(drawPile.begin(), drawPile.end(), g);
    std::cout << "drawPile shuffled\n";

    updateHPText();
    updatePlayerHPText();
    updateArmorText();
    updateEnergyText();
    for (int i = 0; i < 3 && !drawPile.empty(); ++i) {
        drawCard();
    }
}

void BattleScene::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &boardRect);

    if (enemyHPText) {
        int texW, texH;
        SDL_QueryTexture(enemyHPText, nullptr, nullptr, &texW, &texH);
        SDL_Rect textRect = { 350 - texW / 2, 100 - texH / 2, texW, texH };
        SDL_RenderCopy(renderer, enemyHPText, nullptr, &textRect);
    }

    if (playerHPText) {
        int texW, texH;
        SDL_QueryTexture(playerHPText, nullptr, nullptr, &texW, &texH);
        SDL_Rect textRect = { 50, 50, texW, texH };
        SDL_RenderCopy(renderer, playerHPText, nullptr, &textRect);
    }

    if (armorText) {
        int texW, texH;
        SDL_QueryTexture(armorText, nullptr, nullptr, &texW, &texH);
        SDL_Rect textRect = { 50, 80, texW, texH };
        SDL_RenderCopy(renderer, armorText, nullptr, &textRect);
    }

    if (energyText) {
        int texW, texH;
        SDL_QueryTexture(energyText, nullptr, nullptr, &texW, &texH);
        SDL_Rect textRect = { 50, 110, texW, texH };
        SDL_RenderCopy(renderer, energyText, nullptr, &textRect);
    }

    Card* magnifiedCard = nullptr;
    for (auto& card : hand) {
        if (card.getIsMagnified() && !card.isDragging) {
            magnifiedCard = &card;
            break;
        }
    }

    for (auto& card : hand) {
        card.render(renderer, playerEnergy, game->getWindowWidth(), game->getWindowHeight());
    }

    if (magnifiedCard) {
        magnifiedCard->render(renderer, playerEnergy, game->getWindowWidth(), game->getWindowHeight());
    }

    if (battleWon) {
        continueButton.render();
    }

    skipTurnButton.render();

    SDL_RenderPresent(renderer);
}

void BattleScene::handleEvent(SDL_Event& e) {
    if (battleWon && !readyToEnd) {
        continueButton.handleEvent(e);
        return;
    }
    if (playerDefeated) {
        readyToEnd = true; // Allow immediate transition if player is defeated
        return;
    }

    skipTurnButton.handleEvent(e);

    Card* draggingCard = nullptr;
    for (auto& card : hand) {
        if (card.isDragging) {
            draggingCard = &card;
            break;
        }
    }

    if (!draggingCard) {
        for (auto& card : hand) {
            card.handleEvent(e);
        }
    }
    else {
        draggingCard->handleEvent(e);
    }

    if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        for (auto it = hand.begin(); it != hand.end(); ++it) {
            if (it->isDragging) {
                it->isDragging = false;

                SDL_Rect cardRect = it->getRect();
                bool onBoard = (cardRect.x + cardRect.w >= boardRect.x && cardRect.x <= boardRect.x + boardRect.w &&
                    cardRect.y + cardRect.h >= boardRect.y && cardRect.y <= boardRect.y + boardRect.h);
                bool hasEnoughEnergy = (playerEnergy >= it->getEnergyCost());

                if (onBoard && hasEnoughEnergy) {
                    playerEnergy -= it->getEnergyCost();
                    updateEnergyText();
                    playCard(*it);
                    hand.erase(it);
                    updateCardPositions();
                }
                else {
                    it->resetPosition();
                }
                break;
            }
        }
    }

    bool anyDragging = false;
    for (const auto& card : hand) {
        if (card.isDragging) {
            anyDragging = true;
            break;
        }
    }
    if (anyDragging) {
        render();
    }
}

bool BattleScene::isBattleOver() const {
    return battleWon || playerDefeated;
}

bool BattleScene::hasPlayerWon() const {
    return battleWon;
}

bool BattleScene::isReadyToEnd() const {
    return readyToEnd;
}

void BattleScene::updateHPText() {
    if (enemyHPText) SDL_DestroyTexture(enemyHPText);
    std::string hpText = enemy.name + " HP: " + std::to_string(enemy.hp);
    SDL_Color textColor = { 0, 0, 0, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, hpText.c_str(), textColor);
    enemyHPText = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

void BattleScene::updatePlayerHPText() {
    if (playerHPText) SDL_DestroyTexture(playerHPText);
    std::string hpText = "Player HP: " + std::to_string(playerHP);
    SDL_Color textColor = { 0, 0, 0, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, hpText.c_str(), textColor);
    playerHPText = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

void BattleScene::updateArmorText() {
    if (armorText) SDL_DestroyTexture(armorText);
    std::string armorStr = "Armor: " + std::to_string(playerArmor);
    SDL_Color textColor = { 0, 0, 0, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, armorStr.c_str(), textColor);
    armorText = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

void BattleScene::updateEnergyText() {
    if (energyText) SDL_DestroyTexture(energyText);
    std::string energyStr = "Energy: " + std::to_string(playerEnergy) + "/" + std::to_string(maxEnergy);
    SDL_Color textColor = { 0, 0, 0, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, energyStr.c_str(), textColor);
    energyText = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

void BattleScene::drawCard() {
    if (drawPile.empty() && !discard.empty()) {
        drawPile = std::move(discard);
        discard.clear();
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(drawPile.begin(), drawPile.end(), g);
    }
    if (!drawPile.empty() && hand.size() < 5) {
        hand.push_back(std::move(drawPile.back()));
        drawPile.pop_back();
        updateCardPositions();
    }
}

void BattleScene::updateCardPositions() {
    for (size_t i = 0; i < hand.size(); ++i) {
        Card& card = hand[i];
        int newX = 50 + (i + 1) * 110;
        SDL_Rect newRect = { newX, 450, 100, 150 };
        card.getRect() = newRect;
        card.getOriginalRect() = newRect;
        std::cout << "Positioned " << card.getName() << " at (" << newRect.x << ", " << newRect.y << ")\n";
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
        std::cout << "Gained " << effect.value << " armor, total: " << playerArmor << std::endl;
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
    case CardEffectType::Wet:
        applyWetEffect(effect.count);
        break;
    case CardEffectType::Lightning:
        applyLightningEffect(effect.value);
        break;
    case CardEffectType::Ice:
        applyIceEffect(effect.value, effect.count);
        break;
    default:
        break;
    }

    discard.push_back(std::move(card));
    if (enemy.hp <= 0) battleWon = true;
}

void BattleScene::applyWeakenEffect(int value, int turns) {
    enemy.damageReduction = value;
    enemy.weakenTurns = turns;
    std::cout << enemy.name << " is weakened by " << value << " for " << turns << " turns\n";
}

void BattleScene::applyPoisonEffect(int damage, int turns) {
    enemy.poisonDamage = damage;
    enemy.poisonTurns = turns;
    std::cout << enemy.name << " is poisoned for " << damage << " damage over " << turns << " turns\n";
}

void BattleScene::applyThornsEffect() {
    int damage = 3;
    enemy.hp -= damage;
    std::cout << enemy.name << " takes " << damage << " damage from Thorns\n";
    updateHPText();
    if (enemy.hp <= 0) battleWon = true;
}

void BattleScene::applyWetEffect(int turns) {
    enemy.wetTurns = turns;
    std::cout << enemy.name << " is now Wet for " << turns << " turns\n";
}

void BattleScene::applyLightningEffect(int damage) {
    int finalDamage = (enemy.wetTurns > 0) ? damage * 2 : damage;
    enemy.hp -= finalDamage;
    std::cout << "Thunder Strike deals " << finalDamage << " lightning damage, " << enemy.name << " HP now: " << enemy.hp << "\n";
    updateHPText();
}

void BattleScene::applyIceEffect(int damage, int freezeTurns) {
    enemy.hp -= damage;
    if (enemy.wetTurns > 0) {
        enemy.frozen = true;
        std::cout << enemy.name << " is Frozen for " << freezeTurns << " turn(s) due to Wet status\n";
    }
    std::cout << "Ice Shard deals " << damage << " ice damage, " << enemy.name << " HP now: " << enemy.hp << "\n";
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
        updateHPText();
        if (enemy.hp <= 0) battleWon = true;
    }
    if (enemy.wetTurns > 0) {
        enemy.wetTurns--;
        std::cout << enemy.name << " Wet turns remaining: " << enemy.wetTurns << "\n";
    }
}

void BattleScene::enemyAttack() {
    if (enemy.frozen) {
        std::cout << enemy.name << " is Frozen and skips its turn\n";
        enemy.frozen = false;
        return;
    }
    int effectiveDamage = std::max(0, enemy.damage - enemy.damageReduction);
    int damageAfterArmor = std::max(0, effectiveDamage - playerArmor);
    playerArmor = std::max(0, playerArmor - effectiveDamage);
    playerHP -= damageAfterArmor;
    std::cout << enemy.name << " attacks for " << effectiveDamage << " damage, reduced by "
        << (effectiveDamage - damageAfterArmor) << " armor, player HP now: " << playerHP
        << ", armor now: " << playerArmor << std::endl;
    updatePlayerHPText();
    updateArmorText();
    if (playerHP <= 0) playerDefeated = true;
}

void BattleScene::endTurn() {
    if (!battleWon && !playerDefeated) {
        enemyAttack();
    }
    updateEnemyEffects();

    const int desiredHandSize = 3;
    const int maxHandSize = 5;
    int cardsToDraw = (hand.size() < desiredHandSize) ? (desiredHandSize - hand.size()) : 1;
    cardsToDraw = std::min(cardsToDraw, maxHandSize - static_cast<int>(hand.size()));
    for (int i = 0; i < cardsToDraw; ++i) {
        drawCard();
    }
    resetTurn();
}

void BattleScene::resetTurn() {
    playerEnergy = maxEnergy;
    updateEnergyText();
}