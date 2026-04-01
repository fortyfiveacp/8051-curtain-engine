#pragma once
#include <SDL3/SDL.h>

#include "World.h"
#include "manager/AudioManager.h"
#include "manager/SceneManager.h"

struct GameState {
	int hiScore;
	int score;
	int playerHealth;
	int playerBombs;
	int power;
	int graze;
	int point;
};

class Game {
public:
	Game();
	~Game();

	// These are the types that SDL uses
	void init(const char* title, int width, int height, bool fullscreen);

	// Game loop functions (handleEvents, update, render)
	void handleEvents(); // Checks for inputs and system events
	void update(float deltaTime); // Handles game logic and changes to game state
	void render(); // Handles the drawing of the current game state to screen

	// Used to free resources
	void destroy();

	bool running() {
		return isRunning;
	}

	SDL_Renderer* renderer = nullptr;
	AudioManager audioManager;
	SceneManager sceneManager;
	static std::function<void(std::string)> onSceneChangeRequest;
	static GameState gameState;

private:
	int frameCount = 0;
	bool isRunning = false;

	SDL_Window* window = nullptr;
	SDL_Event event;

	// SDL stores colour channels as 8-bit unsigned
	// Range is 0-255
	Uint8 r, g, b, a;
};
