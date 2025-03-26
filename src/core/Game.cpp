#include "Game.h"
#include "../common/Constants.h"
#include "../scenes/MenuScene.h"
#include "../scenes/DeckSelectionScene.h"
#include "../scenes/GameScene.h"
#include "../scenes/BattleScene.h"
#include "../scenes/RewardScene.h"
#include "../scenes/OptionsScene.h"
#include <iostream>
#include <random>

Game::Game() : isRunning(false), window(nullptr), renderer(nullptr), font(nullptr),
currentState(GameState::MENU), currentScene(nullptr), selectedDeckType(DeckType::DAMAGE),
currentNodeIndex(0), isCleaned(false),
windowWidth(Constants::DEFAULT_WINDOW_WIDTH), windowHeight(Constants::DEFAULT_WINDOW_HEIGHT), fullScreen(false) {
}

Game::~Game() {
    clean();
}

bool Game::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    font = TTF_OpenFont(Constants::FONT_PATH.c_str(), Constants::FONT_SIZE);
    if (!font) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }
    std::cout << "Font 'assets/Arial.TTF' loaded successfully" << std::endl;

    initializeCards();

    currentState = GameState::MENU;
    menuScene = std::make_unique<MenuScene>(renderer, font, this);
    currentScene = menuScene.get();
    isRunning = true;
    return true;
}

void Game::reloadFont() {
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    font = TTF_OpenFont(Constants::FONT_PATH.c_str(), Constants::FONT_SIZE);
    if (!font) {
        std::cerr << "Failed to reload font! TTF_Error: " << TTF_GetError() << std::endl;
    }
}

void Game::setResolution(int width, int height) {
    windowWidth = width;
    windowHeight = height;

    // Destroy the old renderer and window
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    // Recreate the window with the new resolution
    Uint32 flags = SDL_WINDOW_SHOWN;
    if (fullScreen) {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    window = SDL_CreateWindow("Roguelike Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, flags);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
        return;
    }

    // Recreate the renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << "\n";
        return;
    }

    // Reload the font
    reloadFont();

    // Update the renderer and font in all scenes
    if (menuScene) {
        auto* scene = dynamic_cast<MenuScene*>(menuScene.get());
        scene->setRenderer(renderer);
        scene->setFont(font);
    }
    if (deckSelectionScene) {
        auto* scene = dynamic_cast<DeckSelectionScene*>(deckSelectionScene.get());
        scene->setRenderer(renderer);
        scene->setFont(font);
    }
    if (gameScene) {
        gameScene->setRenderer(renderer);
        gameScene->setFont(font);
    }
    if (battleScene) {
        auto* scene = dynamic_cast<BattleScene*>(battleScene.get());
        scene->setRenderer(renderer);
        scene->setFont(font);
    }
    if (rewardScene) {
        auto* scene = dynamic_cast<RewardScene*>(rewardScene.get());
        scene->setRenderer(renderer);
        scene->setFont(font);
    }
    if (optionsScene) {
        auto* scene = dynamic_cast<OptionsScene*>(optionsScene.get());
        scene->setRenderer(renderer);
        scene->setFont(font);
    }

    // Reload textures in TextureManager
    textureManager.clear();
    initializeCards(); // Reload card textures with the new renderer
}

void Game::setFullScreen(bool fullScreen) {
    this->fullScreen = fullScreen;
    SDL_SetWindowFullscreen(window, fullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            isRunning = false;
        }
        if (currentScene) {
            currentScene->handleEvent(e);
        }
    }

    if (currentState == GameState::BATTLE) {
        auto* battleScenePtr = dynamic_cast<BattleScene*>(currentScene);
        if (battleScenePtr && battleScenePtr->isBattleOver() && battleScenePtr->isReadyToEnd()) {
            bool won = battleScenePtr->hasPlayerWon();
            handleBattleCompletion(won);
        }
    }

    if (currentState == GameState::GAME) {
        auto* gameScenePtr = dynamic_cast<GameScene*>(currentScene);
        if (gameScenePtr && gameScenePtr->isGameOver()) {
            setState(GameState::MENU);
        }
    }
}

void Game::render() {
    if (currentScene) {
        currentScene->render();
    }
}

void Game::clean() {
    if (isCleaned) {
        return;
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    TTF_Quit();

    IMG_Quit();
    SDL_Quit();

    isCleaned = true;
}

void Game::setState(GameState newState) {
    currentState = newState;
    if (currentState == GameState::MENU) {
        menuScene = std::make_unique<MenuScene>(renderer, font, this);
        currentScene = menuScene.get();
        gameScene.reset();
        deckSelectionScene.reset();
        battleScene.reset();
        rewardScene.reset();
        optionsScene.reset();
        completedNodes.clear();
    }
    else if (currentState == GameState::DECK_SELECTION) {
        deckSelectionScene = std::make_unique<DeckSelectionScene>(renderer, font, this);
        currentScene = deckSelectionScene.get();
        gameScene.reset();
        menuScene.reset();
        battleScene.reset();
        rewardScene.reset();
        optionsScene.reset();
    }
    else if (currentState == GameState::GAME) {
        if (!gameScene) {
            gameScene = std::make_unique<GameScene>(renderer, font, this);
        }
        currentScene = gameScene.get();
        menuScene.reset();
        deckSelectionScene.reset();
        battleScene.reset();
        rewardScene.reset();
        optionsScene.reset();
        gameScene->updateProgression();
    }
    else if (currentState == GameState::BATTLE) {
        menuScene.reset();
        deckSelectionScene.reset();
        rewardScene.reset();
        optionsScene.reset();
    }
    else if (currentState == GameState::REWARD) {
        currentScene = rewardScene.get();
        menuScene.reset();
        deckSelectionScene.reset();
        battleScene.reset();
        optionsScene.reset();
    }
    else if (currentState == GameState::OPTIONS) {
        optionsScene = std::make_unique<OptionsScene>(renderer, font, this);
        currentScene = optionsScene.get();
        menuScene.reset();
        deckSelectionScene.reset();
        gameScene.reset();
        battleScene.reset();
        rewardScene.reset();
    }
}

void Game::selectDeck(DeckType deck) {
    selectedDeckType = deck;
    selectedDeck.clear();
    if (deck == DeckType::DAMAGE) {
        selectedDeck.emplace_back(0, 0, "Slash", 8, 2, renderer, font);
        selectedDeck.emplace_back(0, 0, "Strike", 5, 1, renderer, font);
        selectedDeck.emplace_back(0, 0, "Block", 0, 1, renderer, font, CardEffect(CardEffectType::Armor, 5));
        selectedDeck.emplace_back(0, 0, "Block", 0, 1, renderer, font, CardEffect(CardEffectType::Armor, 5));
    }
    else if (deck == DeckType::BALANCED) {
        selectedDeck.emplace_back(0, 0, "Strike", 5, 1, renderer, font);
        selectedDeck.emplace_back(0, 0, "Block", 0, 1, renderer, font, CardEffect(CardEffectType::Armor, 5));
        selectedDeck.emplace_back(0, 0, "Water Gun", 1, 1, renderer, font, CardEffect(CardEffectType::Wet, 0, 2));
        selectedDeck.emplace_back(0, 0, "Lightning Strike", 6, 3, renderer, font, CardEffect(CardEffectType::Lightning, 6));
        selectedDeck.emplace_back(0, 0, "Ice Shard", 4, 2, renderer, font, CardEffect(CardEffectType::Ice, 4, 1));
    }
    else if (deck == DeckType::ELEMENTAL) {
        selectedDeck.emplace_back(0, 0, "Water Gun", 0, 1, renderer, font, CardEffect(CardEffectType::Wet, 0, 2));
        selectedDeck.emplace_back(0, 0, "Lightning Strike", 6, 3, renderer, font, CardEffect(CardEffectType::Lightning, 6));
        selectedDeck.emplace_back(0, 0, "Ice Shard", 4, 2, renderer, font, CardEffect(CardEffectType::Ice, 4, 1));
        selectedDeck.emplace_back(0, 0, "Heal", 0, 2, renderer, font, CardEffect(CardEffectType::Heal, 5));
        selectedDeck.emplace_back(0, 0, "Water Gun", 1, 1, renderer, font, CardEffect(CardEffectType::Wet, 0, 2));
    }
    else if (deck == DeckType::DEFENSE) {
        selectedDeck.emplace_back(0, 0, "Block", 0, 1, renderer, font, CardEffect(CardEffectType::Armor, 5));
        selectedDeck.emplace_back(0, 0, "Superb Shield", 0, 2, renderer, font, CardEffect(CardEffectType::Armor, 10));
        selectedDeck.emplace_back(0, 0, "Heal", 0, 2, renderer, font, CardEffect(CardEffectType::Heal, 5));
        selectedDeck.emplace_back(0, 0, "Thorns", 0, 1, renderer, font, CardEffect(CardEffectType::Thorns));
        selectedDeck.emplace_back(0, 0, "Thorns", 0, 1, renderer, font, CardEffect(CardEffectType::Thorns));
    }
}

void Game::startBattle(const std::string& enemyName, int enemyHP, int enemyDamage) {
    Enemy enemy(enemyName, enemyHP, enemyDamage);
    battleScene = std::make_unique<BattleScene>(renderer, font, enemy, this);
    currentScene = battleScene.get();
    currentState = GameState::BATTLE;
}

void Game::endBattle(bool won) {
    handleBattleCompletion(won);
}

void Game::handleBattleCompletion(bool won) {
    if (won) {
        if (gameScene) {
            gameScene->markNodeAsCompleted(currentNodeIndex);
            gameScene->lockSiblingNodes(currentNodeIndex);
            gameScene->updateProgression();
        }
        setState(GameState::GAME);
    }
    else {
        setState(GameState::MENU);
    }
}

void Game::addCardToDeck(const Card& card) {
    selectedDeck.push_back(card);
    std::cout << "Added " << card.getName() << " to the deck. New deck size: " << selectedDeck.size() << "\n";
}

std::vector<Card> Game::getRewardCards(CardRarity maxRarity, int count) {
    std::vector<Card> rewards;
    std::vector<Card> possibleCards;

    for (const auto& card : allCards) {
        CardRarity cardRarity = CardRarity::Common;
        std::string name = card.getName();
        if (name == "Lightning Strike" || name == "Ice Shard" || name == "Superb Shield") {
            cardRarity = CardRarity::Rare;
        }
        else if (name == "Dragon's Breath") {
            cardRarity = CardRarity::Epic;
        }

        if (static_cast<int>(cardRarity) <= static_cast<int>(maxRarity)) {
            possibleCards.push_back(card);
        }
    }

    if (possibleCards.empty()) {
        std::cerr << "No cards available for max rarity " << static_cast<int>(maxRarity) << "\n";
        return rewards;
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(possibleCards.begin(), possibleCards.end(), g);

    int numRewards = std::min(count, static_cast<int>(possibleCards.size()));
    for (int i = 0; i < numRewards; ++i) {
        rewards.push_back(possibleCards[i]);
    }

    return rewards;
}

void Game::initializeCards() {
    allCards.clear();
    allCards.emplace_back(0, 0, "Slash", 8, 2, renderer, font);
    allCards.emplace_back(0, 0, "Strike", 5, 1, renderer, font);
    allCards.emplace_back(0, 0, "Block", 0, 1, renderer, font, CardEffect(CardEffectType::Armor, 5));
    allCards.emplace_back(0, 0, "Water Gun", 1, 1, renderer, font, CardEffect(CardEffectType::Wet, 0, 2));
    allCards.emplace_back(0, 0, "Lightning Strike", 6, 3, renderer, font, CardEffect(CardEffectType::Lightning, 6));
    allCards.emplace_back(0, 0, "Ice Shard", 4, 2, renderer, font, CardEffect(CardEffectType::Ice, 4, 1));
    allCards.emplace_back(0, 0, "Heal", 0, 2, renderer, font, CardEffect(CardEffectType::Heal, 5));
    allCards.emplace_back(0, 0, "Superb Shield", 0, 2, renderer, font, CardEffect(CardEffectType::Armor, 10));
    allCards.emplace_back(0, 0, "Thorns", 0, 1, renderer, font, CardEffect(CardEffectType::Thorns));
    allCards.emplace_back(0, 0, "Dragon's Breath", 10, 3, renderer, font);

    for (auto& card : allCards) {
        std::string lowercaseName = card.getName();
        for (char& c : lowercaseName) {
            c = std::tolower(c);
        }
        std::replace(lowercaseName.begin(), lowercaseName.end(), ' ', '_');
        std::string imagePath = Constants::CARD_PATH + lowercaseName + Constants::CARD_SUFFIX;
        card.loadImage(imagePath, renderer, textureManager);
    }

    // Reload selectedDeck cards with the new renderer
    for (auto& card : selectedDeck) {
        std::string lowercaseName = card.getName();
        for (char& c : lowercaseName) {
            c = std::tolower(c);
        }
        std::replace(lowercaseName.begin(), lowercaseName.end(), ' ', '_');
        std::string imagePath = Constants::CARD_PATH + lowercaseName + Constants::CARD_SUFFIX;
        card.loadImage(imagePath, renderer, textureManager);
    }
}