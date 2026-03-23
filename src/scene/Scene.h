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
        world.update(dt, e, type);
    }

    void render() {
        world.render();
    }

    World world;

    const std::string& getName() const {
        return name;
    }

private:
    std::string name;
    SceneType type;
    //void createProjectile(Vector2D pos, Vector2D dir, int speed); TODO commented out during tutorial, not needed?

    void initMainMenu(int windowWidth, int windowHeight);
    void initGameplay(const char* mapPath, int windowWidth, int windowHeight);

    Entity& createPauseMenuOverlay(int windowWidth, int windowHeight);
    Entity& createCogButton(int windowWidth, int windowHeight, Entity& overlay);
    void createPauseMenuUComponents(Entity& overlay);
    void toggleOverlayVisibility(Entity& overlay);

    Entity& createPlayerPosLabel();
};
