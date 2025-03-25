#include "Game.h"
#include "../scenes/MenuScene.h"
#include "../scenes/DeckSelectionScene.h"
#include "../scenes/GameScene.h"
#include "../scenes/BattleScene.h"
#include <iostream>

Game::Game() : isRunning(false), window(nullptr), renderer(nullptr), font(nullptr),
currentState(GameState::MENU), currentScene(nullptr), selectedDeckType(DeckType::DAMAGE),
currentNodeIndex(0) {
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

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    font = TTF_OpenFont("assets/Arial.TTF", 24);
    if (!font) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }
    std::cout << "Font 'assets/Arial.TTF' loaded successfully" << std::endl;

    currentState = GameState::MENU;
    currentScene = std::make_unique<MenuScene>(renderer, font, this);
    isRunning = true;
    return true;
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

void Game::cleanup() {
    clean(); 
}

void Game::setState(GameState newState) {
    currentState = newState;
    if (currentState == GameState::MENU) {
        currentScene = std::make_unique<MenuScene>(renderer, font, this);
    }
    else if (currentState == GameState::DECK_SELECTION) {
        currentScene = std::make_unique<DeckSelectionScene>(renderer, font, this);
    }
    else if (currentState == GameState::GAME) {
        currentScene = std::make_unique<GameScene>(renderer, font, this);
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
        selectedDeck.emplace_back(0, 0, "Water Gun", 3, 1, renderer, font, CardEffect(CardEffectType::Wet, 0, 2));
        selectedDeck.emplace_back(0, 0, "Thunder Strike", 5, 2, renderer, font, CardEffect(CardEffectType::Lightning, 5));
        selectedDeck.emplace_back(0, 0, "Ice Shard", 4, 2, renderer, font, CardEffect(CardEffectType::Ice, 4, 1));
    }
    else if (deck == DeckType::ELEMENTAL) {
        selectedDeck.emplace_back(0, 0, "Water Gun", 3, 1, renderer, font, CardEffect(CardEffectType::Wet, 0, 2));
        selectedDeck.emplace_back(0, 0, "Lightning Strike", 5, 2, renderer, font, CardEffect(CardEffectType::Lightning, 5));
        selectedDeck.emplace_back(0, 0, "Ice Shard", 4, 2, renderer, font, CardEffect(CardEffectType::Ice, 4, 1));
        selectedDeck.emplace_back(0, 0, "Heal", 0, 2, renderer, font, CardEffect(CardEffectType::Heal, 5));
        selectedDeck.emplace_back(0, 0, "Water Gun", 0, 1, renderer, font, CardEffect(CardEffectType::Wet, 0, 2));
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
    currentScene = std::make_unique<BattleScene>(renderer, font, enemy, this);
    currentState = GameState::BATTLE;
}

void Game::endBattle(bool won) {
    if (won) {
        setState(GameState::GAME);
        if (currentState == GameState::GAME && currentScene) {
            GameScene* gameScene = dynamic_cast<GameScene*>(currentScene.get());
            if (gameScene) {
                gameScene->unlockNextNode();
            }
        }
    }
    else {
        setState(GameState::MENU);
    }
}