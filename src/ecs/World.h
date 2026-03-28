#pragma once
#include <memory>
#include <vector>

#include "AnimationSystem.h"
#include "BackgroundRenderSystem.h"
#include "CameraSystem.h"
#include "CollisionSystem.h"
#include "DestructionSystem.h"
#include "Entity.h"
#include "EventResponseSystem.h"
#include "FPSCounterSystem.h"
#include "HUDSystem.h"
#include "IconLabelSystem.h"
#include "event/EventManager.h"
#include "KeyboardInputSystem.h"
#include "MainMenuSystem.h"
#include "Map.h"
#include "MovementSystem.h"
#include "PauseMenuSystem.h"
#include "RenderSystem.h"
#include "SpawnTimerSystem.h"
#include "TimelineSystem.h"
#include "UIRenderSystem.h"
#include "scene/SceneType.h"
#include "PreRenderSystem.h"
#include "StageBackgroundSystem.h"
#include "StageUtils.h"
#include "event/AudioEventQueue.h"

class World {
    Map map;
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<Entity>> deferredEntities;
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
    IconLabelSystem iconLabelSystem;
    PreRenderSystem preRenderSystem;
    BackgroundRenderSystem backgroundRenderSystem;
    StageBackgroundSystem stageBackgroundSystem;
    AudioEventQueue audioEventQueue;

    // Reactive systems
    EventResponseSystem eventResponseSystem{*this};

public:
    World() = default;

    void update(float dt, const SDL_Event& event, SceneType sceneType) {
        if (sceneType == SceneType::MainMenu) {
            // Main menu system update
            mainMenuSystem.update(event);
        } else {
            pauseMenuSystem.update(entities, event);
            keyboardInputSystem.update(entities, event);
            movementSystem.update(entities, dt);
            collisionSystem.update(*this);
            animationSystem.update(entities, dt);
            cameraSystem.update(entities);
            spawnTimerSystem.update(entities, dt);
            timelineSystem.update(entities, dt);
            stageBackgroundSystem.update(entities, dt);
            destructionSystem.update(entities);
            hudSystem.update(entities);
            iconLabelSystem.update(entities);
        }

        fpsCounterSystem.update(entities, dt);
        audioEventQueue.process(); // Process all the audio events.
        preRenderSystem.update(entities);

        synchronizeEntities();
        cleanup();
    }

    void render(SDL_Renderer* renderer, int windowWidth, int windowHeight) {
        backgroundRenderSystem.render(entities);

        // Set up stage viewport.
        int stageWidth = StageUtils::CalculateStageWidth(windowWidth);
        int stageHeight = StageUtils::CalculateStageHeight(windowHeight);
        int paddingX = StageUtils::CalculateStagePaddingX(windowWidth);
        int paddingY = StageUtils::CalculateStagePaddingY(windowHeight);

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
            // Push back all deferred entities to the entities vector
            // Using move so we don't create a copy
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

    AudioEventQueue& getAudioEventQueue() {
        return audioEventQueue;
    }

    Map& getMap() {
        return map;
    }
};
