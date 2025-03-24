#include "Game.h"
#include <iostream>
#include <cstdlib> 

Game::Game()
    : window(nullptr), renderer(nullptr), font(nullptr), isRunning(false),
    currentState(GameState::MENU), currentScene(nullptr), currentNodeIndex(0) {
    selectedDeck.reserve(7); 
}

bool Game::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }
    if (TTF_Init() < 0) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
        return false;
    }
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }
    font = TTF_OpenFont("assets/Arial.TTF", 24);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return false;
    }
    isRunning = true;
    currentScene = std::make_unique<MenuScene>(renderer, font, this);
    currentNodeIndex = 0; 
    return true;
}

Game::~Game() {
    clean();
}

void Game::run() {
    while (isRunning) {
        handleEvents();
        update();
        render();
    }
}

void Game::clean() {
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

void Game::setGameState(GameState newState) {
    currentState = newState;
    if (currentState == GameState::MENU) {
        currentScene = std::make_unique<MenuScene>(renderer, font, this);
    }
    else if (currentState == GameState::DECK_SELECTION) {
        currentScene = std::make_unique<DeckSelectionScene>(renderer, font, this);
    }
    else if (currentState == GameState::PLAYING) {
        currentScene = std::make_unique<GameScene>(renderer, font, this);
    }
    // BATTLE handled by startBattle
}

void Game::setSelectedDeck(int deckId) {
    selectedDeck.clear();
    switch (deckId) {
    case 0: // Damage Deck
        selectedDeck.emplace_back(0, 0, "Strike", 5, 1, renderer, font);
        selectedDeck.emplace_back(0, 0, "Strike", 5, 1, renderer, font);
        selectedDeck.emplace_back(0, 0, "Strike", 5, 1, renderer, font);
        selectedDeck.emplace_back(0, 0, "Slash", 8, 2, renderer, font);
        selectedDeck.emplace_back(0, 0, "Slash", 8, 2, renderer, font);
        selectedDeck.emplace_back(0, 0, "MultiStrike", 0, 2, renderer, font, CardEffect(CardEffectType::MultiStrike, 3, 3));
        selectedDeck.emplace_back(0, 0, "MultiStrike", 0, 2, renderer, font, CardEffect(CardEffectType::MultiStrike, 3, 3));
        break;
    case 1: // DoT Deck
        selectedDeck.emplace_back(0, 0, "Strike", 5, 1, renderer, font);
        selectedDeck.emplace_back(0, 0, "Strike", 5, 1, renderer, font);
        selectedDeck.emplace_back(0, 0, "Poison", 0, 1, renderer, font, CardEffect(CardEffectType::Poison, 2, 3));
        selectedDeck.emplace_back(0, 0, "Poison", 0, 1, renderer, font, CardEffect(CardEffectType::Poison, 2, 3));
        selectedDeck.emplace_back(0, 0, "Poison", 0, 1, renderer, font, CardEffect(CardEffectType::Poison, 2, 3));
        selectedDeck.emplace_back(0, 0, "Weaken", 0, 1, renderer, font, CardEffect(CardEffectType::Weaken, 2, 2));
        selectedDeck.emplace_back(0, 0, "Weaken", 0, 1, renderer, font, CardEffect(CardEffectType::Weaken, 2, 2));
        break;
    case 2: // Defense Deck
        selectedDeck.emplace_back(0, 0, "Block", 0, 1, renderer, font, CardEffect(CardEffectType::Armor, 5));
        selectedDeck.emplace_back(0, 0, "Block", 0, 1, renderer, font, CardEffect(CardEffectType::Armor, 5));
        selectedDeck.emplace_back(0, 0, "Block", 0, 1, renderer, font, CardEffect(CardEffectType::Armor, 5));
        selectedDeck.emplace_back(0, 0, "Superb Shield", 0, 1, renderer, font, CardEffect(CardEffectType::Armor, 7, 2));
        selectedDeck.emplace_back(0, 0, "Thorns", 0, 2, renderer, font, CardEffect(CardEffectType::Thorns, 0, 1));
        selectedDeck.emplace_back(0, 0, "Thorns", 0, 2, renderer, font, CardEffect(CardEffectType::Thorns, 0, 1));
        selectedDeck.emplace_back(0, 0, "Strike", 5, 1, renderer, font);
        selectedDeck.emplace_back(0, 0, "Strike", 5, 1, renderer, font);
        break;
    case 3: // Balanced Deck
        selectedDeck.emplace_back(0, 0, "Strike", 5, 1, renderer, font);
        selectedDeck.emplace_back(0, 0, "Strike", 5, 1, renderer, font);
        selectedDeck.emplace_back(0, 0, "Block", 0, 1, renderer, font, CardEffect(CardEffectType::Armor, 5));
        selectedDeck.emplace_back(0, 0, "Heal", 0, 1, renderer, font, CardEffect(CardEffectType::Heal, 5));
        selectedDeck.emplace_back(0, 0, "Weaken", 0, 1, renderer, font, CardEffect(CardEffectType::Weaken, 2, 2));
        selectedDeck.emplace_back(0, 0, "Poison", 0, 1, renderer, font, CardEffect(CardEffectType::Poison, 2, 3));
        selectedDeck.emplace_back(0, 0, "Thorns", 0, 2, renderer, font, CardEffect(CardEffectType::Thorns, 0, 1));
        break;
    }
}

void Game::startBattle(const Enemy& enemy) {
    currentScene = std::make_unique<BattleScene>(renderer, font, enemy, this);
    currentState = GameState::BATTLE;
}

void Game::endBattle(bool won) {
    if (won) {
        currentNodeIndex++; 
        currentScene = std::make_unique<GameScene>(renderer, font, this);
        std::cout << "Battle won, returning to map, currentNodeIndex: " << currentNodeIndex << "\n";
        currentState = GameState::PLAYING;
    }
    else {
        std::cout << "Game Over!\n";
        currentState = GameState::MENU;
        currentScene = std::make_unique<MenuScene>(renderer, font, this);
        currentNodeIndex = 0; 
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }
        if (currentScene) {
            currentScene->handleInput(&event);
        }
    }
}

void Game::update() {
    if (currentScene) {
        currentScene->update();
    }
}

void Game::render() {
    if (currentScene) {
        currentScene->render(renderer);
        SDL_RenderPresent(renderer);
    }
}