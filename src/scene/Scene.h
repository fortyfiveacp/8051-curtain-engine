#pragma once
#include <string>

#include "World.h"
#include "SDL3/SDL_events.h"
#include "SceneType.h"

class Scene {
public:
    Scene(SceneType sceneType, const char* sceneName, const char* mapPath, int windowWidth, int windowHeight);

    void update(const float dt, const SDL_Event &e) {
        world.update(dt, e, type, isPaused, isDebugging);
    }

    void render(SDL_Renderer* renderer, int windowWidth, int windowHeight) {
        world.render(renderer, windowWidth, windowHeight, isDebugging);
    }

    World world;

    const std::string& getName() const {
        return name;
    }
private:
    std::string name;
    SceneType type;
    bool isPaused = false;
    bool isDebugging = false;
    //void createProjectile(Vector2D pos, Vector2D dir, int speed); TODO: commented out during tutorial, not needed?

    void initMainMenu(int windowWidth, int windowHeight);
    void initGameplay(const char* mapPath, int windowWidth, int windowHeight);

    // Functions for creating the specific UI in the game.
    void createSidebarUILabels(int windowWidth, int windowHeight, float stageWidth, float stageHeight);
    void createPauseMenuUComponents(Entity& overlay, int windowWidth, int windowHeight);
    void createContinueGameUIComponents(Entity& overlay, int windowWidth, int windowHeight);
    void createOverlayUIComponents(Entity& overlay, int windowWidth, int windowHeight, const char* titleText,
        const char* titleCacheKey, const std::vector<Entity*> &selectableButtons);
};
