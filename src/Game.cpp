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

	// Initialize SDL library.
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) == 1) {
		std::cout << "Subsystem initialized..." << std::endl;
		window = SDL_CreateWindow(title, width, height, flags);
		
		if (window) {
			std::cout << "Window created..." << std::endl;
		}

		// Windows will be DirectX.
		// Mac will likely be Metal, OpenGL.
		renderer = SDL_CreateRenderer(window, nullptr);

		if (renderer) {
			std::cout << "Renderer created..." << std::endl;
		}
		else {
			std::cout << "Renderer could not be created..." << std::endl;
		}

		if (TTF_Init() != 1) {
			std::cout << "TTF_Init failed..." << std::endl;
		}

		isRunning = true;
	} else {
		isRunning = false;
	}

	// Load audio.
	audioManager.loadAudio("menu-theme", "../asset/audio/music/Menu Theme.flac");
	audioManager.loadAudio("stage-theme", "../asset/audio/music/Stage Theme.flac");
	audioManager.loadAudio("boss-theme", "../asset/audio/music/Boss Theme.flac");
	audioManager.loadAudio("credits-theme", "../asset/audio/music/Credits Theme.flac");

	// Load sfx.
	audioManager.loadAudio("player-hit", "../asset/audio/sfx/se_pldead00.flac");
	audioManager.loadAudio("pause", "../asset/audio/sfx/se_pause.flac");
	audioManager.loadAudio("select", "../asset/audio/sfx/se_select00.flac");
	audioManager.loadAudio("ok", "../asset/audio/sfx/se_ok00.flac");
	audioManager.loadAudio("bomb", "../asset/audio/sfx/se_slash.flac");
	audioManager.loadAudio("item", "../asset/audio/sfx/se_item00.flac");
	audioManager.loadAudio("circle-bullet-shot", "../asset/audio/sfx/se_tan01.flac");
	audioManager.loadAudio("enemy-hit", "../asset/audio/sfx/se_damage00.flac");
	audioManager.loadAudio("enemy-hit-low", "../asset/audio/sfx/se_damage01.flac");
	audioManager.loadAudio("enemy-dead", "../asset/audio/sfx/se_enep00.flac");
	audioManager.loadAudio("boss-transition", "../asset/audio/sfx/se_cat00.flac");
	audioManager.loadAudio("boss-dead", "../asset/audio/sfx/se_enep01.flac");

	// Load fonts.
	AssetManager::loadFont("pop1", "../asset/fonts/pop1-w9.ttf", 32);
	AssetManager::loadFont("DFPPOPCorn", "../asset/fonts/DFPPOPCORN-W12.ttf", 32);
	AssetManager::loadFont("tiranti", "../asset/fonts/Tiranti Solid LET Plain.ttf", 30);

	// Load animations.
	AssetManager::loadAnimation("player", "../asset/animations/reimu_animations.xml");
	AssetManager::loadAnimation("enemy", "../asset/animations/bird_animations.xml");
	AssetManager::loadAnimation("sanae", "../asset/animations/sanae_animations.xml");
	AssetManager::loadAnimation("redFairy", "../asset/animations/red_fairy_animations.xml");
	AssetManager::loadAnimation("blueFairy", "../asset/animations/blue_fairy_animations.xml");
	AssetManager::loadAnimation("largeFairy", "../asset/animations/large_fairy_animations.xml");

	// Load scenes.
	sceneManager.loadScene(
		SceneType::MainMenu,
		"mainmenu",
		nullptr,
		nullptr,
		nullptr,
		width,
		height
	);

	sceneManager.loadScene(
		SceneType::Gameplay,
		"level1",
		"../asset/stage/stage1.xml",
		"../asset/stage/stage1.png",
		"../asset/stage/foreground1.png",
		width,
		height
	);

	sceneManager.loadScene(
		SceneType::Credits,
		"credits",
		nullptr,
		nullptr,
		nullptr,
		width,
		height
	);

	// Init game data / state.
	gameState.hiScore = 0;
	gameState.score = 0;
	gameState.playerHealth = 3;
	gameState.playerBombs = 3;
	gameState.power = 0;
	gameState.graze = 0;
	gameState.point = 0;
	gameState.continues = 0;

	// Start music.
	// audioManager.playMusic("menu-theme");

	// Start level 1.
	sceneManager.changeSceneDeferred("mainmenu");

	// Resolve scene callback.
	onSceneChangeRequest = [this](std::string sceneName) {
		// TODO purge.
		// Some game state happening here.
		// if (sceneManager.currentScene->getName() == "level2" && sceneName == "level2") {
		// 	std::cout << "You win!" << std::endl;
		// 	isRunning = false;
		// 	return;
		// }
		//
		// if (sceneName == "gameover") {
		// 	std::cout << "Mission failed we'll get'em next time" << std::endl;
		// 	isRunning = false;
		// 	return;
		// }

		sceneManager.changeSceneDeferred(sceneName);
	};
}

void Game::handleEvents() {
	// SDL listens to the OS for input events internally and adds them to a queue.
	// SDL_Event event;

	// Check for next event, if an event is available it will remove from the queue and store in event variable.
	SDL_PollEvent(&event);

	switch (event.type) {
		case SDL_EVENT_QUIT:
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
	r = 255;
	g = 255;
	b = 255;
	a = 255;

	// TODO: purge
	// r = (frameCount / 120 * 15) % 255;
	// g = (frameCount / 120 * 36) % 255;
	// b = (frameCount / 120 * 55) % 255;
	// a = 255;

	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	// Every frame the renderer is cleared with the draw colour.
	SDL_RenderClear(renderer);

	// Scene does the rendering now
	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	sceneManager.render(renderer, width, height);

	// Display everything that was just drawn.
	// Draws it in memory first to a back buffer.
	// Swaps the back buffer to the screen.
	SDL_RenderPresent(renderer);
}

void Game::destroy() {
	TextureManager::clean();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	std::cout << "Game destroyed" << std::endl;
}