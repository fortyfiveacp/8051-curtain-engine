// 8051Tutorial.cpp : Defines the entry point for the application.
//

#include "Game.h"
#include <iostream>

// Global variable
Game* game = nullptr;

int main()
{
    std::cout << "Hello CMake." << std::endl;

    // Comments are for 60fps
    const int FPS = 120; // 60 is the closest refresh rate of most our monitors, 30 is half the work
    const int desiredFrameTime = 1000 / FPS; // 16ms per frame

    Uint64 ticks = 0;
    float deltaTime = 0.0f;
    int actualFrameTime;

    game = new Game();
    game->init("東方巫女戦場 ～ Double Prayer Duel", 800, 600, false);

    // Game loop
    while (game->running()) {
        int currentTicks = SDL_GetTicks(); // Time in ms since we initialized SDL
        deltaTime = (currentTicks - ticks) / 1000.0f;
        ticks = currentTicks;

        game->handleEvents();
        game->update(deltaTime);
        game->render();

        actualFrameTime = SDL_GetTicks() - ticks; // Elapsed time in ms it took the current frame

        // Frame limiter, keeps game running at desired frame rate
        // If actual frame took less time to draw than desired frame time, delay the difference
        if (desiredFrameTime > actualFrameTime) {
            SDL_Delay(desiredFrameTime - actualFrameTime);
        }
    }

    delete game;

    return 0;
}
