#pragma once
#include <memory>
#include <vector>

#include "AnimationSystem.h"
#include "BackgroundRenderSystem.h"
#include "CameraSystem.h"
#include "CollisionSystem.h"
#include "ConvoySystem.h"
#include "DestructionSystem.h"
#include "Entity.h"
#include "EventResponseSystem.h"
#include "FPSCounterSystem.h"
#include "HUDSystem.h"
#include "event/EventManager.h"
#include "KeyboardInputSystem.h"
#include "MainMenuSystem.h"
#include "Map.h"
#include "MovementSystem.h"
#include "PathSystem.h"
#include "PauseMenuSystem.h"
#include "RenderSystem.h"
#include "SpawnTimerSystem.h"
#include "TimelineSystem.h"
#include "UIRenderSystem.h"
#include "scene/SceneType.h"
#include "PreRenderSystem.h"
#include "StageBackgroundSystem.h"

class World {
    Map map;
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<Entity>> deferredEntities;
    std::unordered_map<int, Path> pathLibrary;
    PathSystem pathSystem{&pathLibrary};
    MovementSystem movementSystem;
    RenderSystem renderSystem;
    KeyboardInputSystem keyboardInputSystem;
    CollisionSystem collisionSystem;
    AnimationSystem animationSystem;
    CameraSystem cameraSystem;
    EventManager eventManager;
    SpawnTimerSystem spawnTimerSystem;
    TimelineSystem timelineSystem;
    DestructionSystem destructionSystem;
    MainMenuSystem mainMenuSystem;
    UIRenderSystem uiRenderSystem;
    PauseMenuSystem pauseMenuSystem;
    HUDSystem hudSystem;
    FPSCounterSystem fpsCounterSystem;
    PreRenderSystem preRenderSystem;
    BackgroundRenderSystem backgroundRenderSystem;
    StageBackgroundSystem stageBackgroundSystem;

    // Reactive systems.
    EventResponseSystem eventResponseSystem{*this};

    ConvoySystem convoySystem;

public:
    World() = default;

    void update(float dt, const SDL_Event& event, SceneType sceneType) {
        if (sceneType == SceneType::MainMenu) {
            // Main menu system update.
            mainMenuSystem.update(event);
        } else {
            convoySystem.update(*this, dt);
            pauseMenuSystem.update(entities, event);
            keyboardInputSystem.update(entities, event);
            pathSystem.update(entities, dt);
            movementSystem.update(entities, dt);
            collisionSystem.update(*this);
            animationSystem.update(entities, dt);
            cameraSystem.update(entities);
            spawnTimerSystem.update(entities, dt);
            timelineSystem.update(entities, dt);
            stageBackgroundSystem.update(entities, dt);
            destructionSystem.update(entities);
            hudSystem.update(entities);
        }

        fpsCounterSystem.update(entities, dt);
        preRenderSystem.update(entities);

        synchronizeEntities();
        cleanup();
    }

    void render(SDL_Renderer* renderer, int windowWidth, int windowHeight) {
        backgroundRenderSystem.render(entities);

        // Set up stage viewport.
        int stageWidth = windowWidth * 0.6;
        int stageHeight = windowHeight * 0.93;
        int paddingX = windowWidth * 0.05;
        int paddingY = (windowHeight - stageHeight) / 2;

        SDL_Rect stageRect = { paddingX, paddingY, stageWidth, stageHeight };
        SDL_SetRenderViewport(renderer, &stageRect);

        // TODO: purge.
        // for (auto& entity : entities) {
        //     if (entity->hasComponent<Camera>()) {
        //         map.draw(entity->getComponent<Camera>());
        //         break;
        //     }
        // }

        renderSystem.render(entities);

        // Reset viewport for rendering UI.
        SDL_SetRenderViewport(renderer, nullptr);

        uiRenderSystem.render(entities);
    }

    Entity& createEntity() {
        // We use emplace instead of push so we don't create a copy.
        entities.emplace_back(std::make_unique<Entity>());
        return *entities.back();
    }

    Entity& createDeferredEntity() {
        deferredEntities.emplace_back(std::make_unique<Entity>());
        return *deferredEntities.back();
    }

    std::vector<std::unique_ptr<Entity>>& getEntities() {
        return entities;
    }

    std::unordered_map<int, Path>& getPathLibrary() {
        return pathLibrary;
    }

    void cleanup() {
        // Use a lambda predicate to remove all inactive entities.
        std::erase_if(
            entities,
            [](std::unique_ptr<Entity>& e) {
                return !e->isActive();
            }
        );
    }

    void synchronizeEntities() {
        if (!deferredEntities.empty()) {
            // Push back all deferred entities to the entities vector.
            // Using move so we don't create a copy.
            std::move(
                deferredEntities.begin(),
                deferredEntities.end(),
                std::back_inserter(entities)
            );

            // Clearing the creation buffer.
            deferredEntities.clear();
        }
    }

    EventManager& getEventManager() {
        return eventManager;
    }

    Map& getMap() {
        return map;
    }
};
