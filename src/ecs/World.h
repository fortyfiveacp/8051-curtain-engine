#pragma once
#include <memory>
#include <vector>

#include "AnimationSystem.h"
#include "CameraSystem.h"
#include "CollisionSystem.h"
#include "DestructionSystem.h"
#include "Entity.h"
#include "EventResponseSystem.h"
#include "event/EventManager.h"
#include "KeyboardInputSystem.h"
#include "MainMenuSystem.h"
#include "Map.h"
#include "MouseInputSystem.h"
#include "MovementSystem.h"
#include "RenderSystem.h"
#include "SpawnTimerSystem.h"
#include "TimelineSystem.h"
#include "UIRenderSystem.h"
#include "scene/SceneType.h"

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
    MouseInputSystem mouseInputSystem; // TODO temp

    // Reactive systems
    EventResponseSystem eventResponseSystem{*this};

public:
    World() = default;

    void update(float dt, const SDL_Event& event, SceneType sceneType) {
        if (sceneType == SceneType::MainMenu) {
            // Main menu system update
            mainMenuSystem.update(event);
        }
        else {
            keyboardInputSystem.update(entities, event);
            movementSystem.update(entities, dt);
            collisionSystem.update(*this);
            animationSystem.update(entities, dt);
            cameraSystem.update(entities);
            spawnTimerSystem.update(entities, dt);
            timelineSystem.update(entities, dt);
            destructionSystem.update(entities);
        }

        mouseInputSystem.update(*this, event);

        synchronizeEntities();
        cleanup();
    }

    void render() {
        for (auto& entity : entities) {
            if (entity->hasComponent<Camera>()) {
                map.draw(entity->getComponent<Camera>());
                break;
            }
        }

        renderSystem.render(entities);
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

    Map& getMap() {
        return map;
    }
};
