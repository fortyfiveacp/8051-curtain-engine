#pragma once
#include <string>

#include "Vector2D.h"
#include "World.h"
#include "SDL3/SDL_events.h"
#include "SceneType.h"

class Scene {
public:
    Scene(SceneType sceneType, const char* sceneName, const char* mapPath, int windowWidth, int windowHeight);

    void update(const float dt, const SDL_Event &e) {
        world.update(dt, e, type, isPaused);
    }

    void render(SDL_Renderer* renderer, int windowWidth, int windowHeight) {
        world.render(renderer, windowWidth, windowHeight);
    }

    World world;

    const std::string& getName() const {
        return name;
    }

    void togglePause() {
        isPaused = !isPaused;
    }

private:
    std::string name;
    SceneType type;
    bool isPaused = false;
    //void createProjectile(Vector2D pos, Vector2D dir, int speed); TODO: commented out during tutorial, not needed?

    void initMainMenu(int windowWidth, int windowHeight);
    void initGameplay(const char* mapPath, int windowWidth, int windowHeight);

    // Functions for creating UI elements.
    Entity& createSelectableButton(Entity& overlay, const char* font, SDL_Color selectedColour, SDL_Color unselectedColour,
        const char* text, const char* cacheKey, const std::function<void()>& onPressed);
    Entity& createLabel(int x, int y, SDL_Color colour, const char* fontName, const char* text, const char* cacheKey,
        LabelType labelType);
    Entity& createIconLabel(int x, int y, int maxNumber, float iconWidth, float iconHeight,
        IconCounterType iconCounterType, const char* texturePath);
    Entity& createStageBackground(float stageWidth, float stageHeight, float startingY, float scrollSpeedY, const char* texturePath);

    // Functions for creating the specific UI in the game.
    void createSidebarUILabels(int windowWidth, int windowHeight, float stageWidth, float stageHeight);
    Entity& createPauseMenuOverlay(int windowWidth, int windowHeight);
    void createPauseMenuUComponents(Entity& overlay, int windowWidth, int windowHeight);

    // UI related helpers.
    void toggleOverlayVisibility(Entity& overlay);
};
