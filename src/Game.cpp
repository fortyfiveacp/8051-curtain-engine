#include "Game.h"
#include "Map.h"

#include <chrono>
#include <iostream>

#include "manager/AssetManager.h"

GameState Game::gameState{};

std::function<void(std::string)> Game::onSceneChangeRequest;

Game::Game() {

}

Game::~Game() {
	destroy();
}

void Game::init(const char* title, int width, int height, bool fullscreen) {
	int flags = 0;
	if (fullscreen) {
		flags = SDL_WINDOW_FULLSCREEN;
	}

	// Initialize SDL library
	if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 1) {
		if (!MIX_Init()) {
			std::cerr << "SDL3_mixer init failed: " << SDL_GetError() << std::endl;
			return;
		} else {
			std::cout << "Mixer initialized" << std::endl;
		}

		std::cout << "Subsystem initialized..." << std::endl;
		window = SDL_CreateWindow(title, width, height, flags);

		if (window) {
			std::cout << "Window created..." << std::endl;
		}

		// Windows will be DirectX
		// Mac will likely be Metal, OpenGL
		renderer = SDL_CreateRenderer(window, nullptr);

		if (renderer) {
			std::cout << "Renderer created..." << std::endl;
		}
		else {
			std::cout << "Renderer could not be created..." << std::endl;
		}

		isRunning = true;
	}
	else {
		isRunning = false;
	}

	// Load assets
	AssetManager::loadAnimation("player", "../asset/animations/fox_animations.xml");
	AssetManager::loadAnimation("enemy", "../asset/animations/bird_animations.xml");

	AssetManager::loadSound(SoundID::PlayerShot, "../asset/audio/sfx/generic_shot.mp3");
	AssetManager::loadMusic(MusicID::StageTheme, "../asset/audio/music/10. The Primal Scene of Japan the Girl Saw.mp3");
	// AssetManager::loadMusic(MusicID::StageTheme, "../asset/audio/music/Interdimensional Voyage of a Ghostly Passenger Ship.wav");

	// Load scenes
	sceneManager.loadScene(SceneType::MainMenu, "mainmenu", nullptr, width, height);
	sceneManager.loadScene(SceneType::Gameplay, "level1", "../asset/map.tmx", width, height);
	sceneManager.loadScene(SceneType::Gameplay, "level2", "../asset/map2.tmx", width, height);

	// Init game data / state
	gameState.playerHealth = 5;

	// Start level 1
	sceneManager.changeSceneDeferred("mainmenu");

	// Resolve scene callback
	onSceneChangeRequest = [this](std::string sceneName) {

		// Some game state happening here
		if (sceneManager.currentScene->getName() == "level2" && sceneName == "level2") {
			std::cout << "You win!" << std::endl;
			isRunning = false;
			return;
		}

		if (sceneName == "gameover") {
			std::cout << "Mission failed we'll get'em next time" << std::endl;
			isRunning = false;
			return;
		}

		sceneManager.changeSceneDeferred(sceneName);
	};
}

void Game::handleEvents() {
	// SDL listens to the OS for input events internally and adds them to a queue
	// SDL_Event event;

	// Check for next event, if an event is available it will remove from the queue and store in event variable
	SDL_PollEvent(&event);

	switch (event.type) {
	case SDL_EVENT_QUIT: // Triggered when user closes window
		isRunning = false;
		break;
	default:
		break;
	}
}

void Game::update(float deltaTime) {
	frameCount++;
	sceneManager.update(deltaTime, event);
	// std::cout << frameCount << std::endl;
}

void Game::render() {
	// r = 100;
	// g = 255;
	// b = 50;
	// a = 255;

	r = (frameCount / 120 * 15) % 255;
	g = (frameCount / 120 * 36) % 255;
	b = (frameCount / 120 * 55) % 255;
	a = 255;

	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	// Every frame the renderer is cleared with the draw colour
	SDL_RenderClear(renderer);

	// Scene does the rendering now
	sceneManager.render();

	// Display everything that was just drawn
	// Draws it in memory first to a back buffer
	// Swaps the back buffer to the screen
	SDL_RenderPresent(renderer);
}

void Game::destroy() {
	TextureManager::clean();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	std::cout << "Game destroyed" << std::endl;
}