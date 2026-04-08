#pragma once
#include <memory>
#include <vector>

#include "AnimationSystem.h"
#include "BackgroundRenderSystem.h"
#include "BossHealthBarSystem.h"
#include "BossMovementSystem.h"
#include "BossStateSystem.h"
#include "BossTrackerSystem.h"
#include "CameraSystem.h"
#include "CollisionSystem.h"
#include "ConvoySystem.h"
#include "DeathBombSystem.h"
#include "DebugRenderSystem.h"
#include "DestructionSystem.h"
#include "EnemyHealthSystem.h"
#include "Entity.h"
#include "EventResponseSystem.h"
#include "FadeSystem.h"
#include "FPSCounterSystem.h"
#include "HUDSystem.h"
#include "IconCounterSystem.h"
#include "event/EventManager.h"
#include "KeyboardInputSystem.h"
#include "LinearSpawnerSystem.h"
#include "MainMenuSystem.h"
#include "Map.h"
#include "MovementSystem.h"
#include "PathSystem.h"
#include "RadialSpawnerSystem.h"
#include "PauseMenuSystem.h"
#include "PlayerAbilitySystem.h"
#include "InvincibilityFramesSystem.h"
#include "ItemBounceSystem.h"
#include "PlayerBombSystem.h"
#include "PlayerBoundsSystem.h"
#include "PlayerFocusRenderSystem.h"
#include "PlayerRespawnSystem.h"
#include "RenderSystem.h"
#include "SpawnTimerSystem.h"
#include "TimelineSystem.h"
#include "UIRenderSystem.h"
#include "scene/SceneType.h"
#include "PreRenderSystem.h"
#include "SelectableUISystem.h"
#include "StageBackgroundSystem.h"
#include "StageUtils.h"
#include "WorldBackgroundRenderSystem.h"
#include "event/AudioEventQueue.h"

class World {
    Map map; // TODO purge.
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<Entity>> deferredEntities;
    std::unordered_map<int, Path> pathLibrary;
    PathSystem pathSystem;
    MovementSystem movementSystem;
    RenderSystem renderSystem;
    KeyboardInputSystem keyboardInputSystem;
    CollisionSystem collisionSystem;
    AnimationSystem animationSystem;
    CameraSystem cameraSystem;
    EventManager eventManager;
    SpawnTimerSystem spawnTimerSystem;
    RadialSpawnerSystem radialSpawnerSystem;
    LinearSpawnerSystem linearSpawnerSystem;
    TimelineSystem timelineSystem;
    DestructionSystem destructionSystem;
    MainMenuSystem mainMenuSystem;
    UIRenderSystem uiRenderSystem;
    PauseMenuSystem pauseMenuSystem;
    HUDSystem hudSystem;
    FPSCounterSystem fpsCounterSystem;
    IconCounterSystem iconLabelSystem;
    PreRenderSystem preRenderSystem;
    BackgroundRenderSystem backgroundRenderSystem;
    StageBackgroundSystem stageBackgroundSystem;
    AudioEventQueue audioEventQueue;
    PlayerAbilitySystem playerAbilitySystem;
    DeathBombSystem deathBombSystem;
    InvincibilityFramesSystem invincibilityFramesSystem;
    SelectableUISystem selectableUISystem;
    DebugRenderSystem debugRenderSystem;
    PlayerBoundsSystem playerBoundsSystem;
    ItemBounceSystem itemBounceSystem;
    PlayerRespawnSystem playerRespawnSystem;
    PlayerBombSystem playerBombSystem;
    FadeSystem fadeSystem;
    BossHealthBarSystem bossHealthBarSystem;
    BossTrackerSystem bossTrackerSystem;
    EnemyHealthSystem enemyHealthSystem;
    WorldBackgroundRenderSystem worldBackgroundRenderSystem;
    PlayerFocusRenderSystem playerFocusRenderSystem;

    // Reactive systems.
    EventResponseSystem eventResponseSystem{*this};

    ConvoySystem convoySystem;
    BossMovementSystem bossMovementSystem;
    BossStateSystem bossStateSystem;

public:
    World() = default;

    void update(float dt, const SDL_Event& event, SceneType sceneType, bool isPaused, bool isDebugging) {
        if (sceneType == SceneType::MainMenu) {
            // Main menu system update.
            mainMenuSystem.update(event);
        } else {
            keyboardInputSystem.update(entities, event);
            cameraSystem.update(entities);
            playerBoundsSystem.update(entities);
            pauseMenuSystem.update(entities, event);
            selectableUISystem.update(entities, *this, event);

            // Only update gameplay systems if the game isn't paused.
            if (!isPaused) {
                movementSystem.update(entities, dt);
                collisionSystem.update(*this);
                deathBombSystem.update(entities, dt);
                playerRespawnSystem.update(entities, dt);
                invincibilityFramesSystem.update(entities, dt);
                playerAbilitySystem.update(*this, dt);
                playerBombSystem.update(entities, dt);
                convoySystem.update(*this, dt);
                pathSystem.update(*this, entities, dt);
                bossTrackerSystem.update(entities);
                animationSystem.update(entities, dt);
                // cameraSystem.update(entities); // TODO: decide what to do with the camera system.
                spawnTimerSystem.update(entities, dt);
                itemBounceSystem.update(entities, dt);
                radialSpawnerSystem.update(entities, dt);
                linearSpawnerSystem.update(entities, dt);
                timelineSystem.update(entities, dt);
                stageBackgroundSystem.update(entities, dt);
                bossHealthBarSystem.update(entities, dt);
                enemyHealthSystem.update(*this);
                bossMovementSystem.update(*this, dt);
                bossStateSystem.update(*this, dt);
            }

            debugRenderSystem.update(*this, event, isDebugging);
            destructionSystem.update(entities);
            hudSystem.update(entities);
            iconLabelSystem.update(entities);
        }

        fpsCounterSystem.update(entities, dt);
        audioEventQueue.process(); // Process all the audio events.
        preRenderSystem.update(entities);
        fadeSystem.update(entities, dt);

        synchronizeEntities();
        cleanup();
    }

    void render(SDL_Renderer* renderer, int windowWidth, int windowHeight, bool isDebugging) {
        backgroundRenderSystem.render(entities);

        // Set up stage viewport.
        int stageWidth = StageUtils::CalculateStageWidth(windowWidth);
        int stageHeight = StageUtils::CalculateStageHeight(windowHeight);
        int paddingX = StageUtils::CalculateStagePaddingX(windowWidth);
        int paddingY = StageUtils::CalculateStagePaddingY(windowHeight);

        SDL_Rect stageRect = { paddingX, paddingY, stageWidth, stageHeight };
        SDL_SetRenderViewport(renderer, &stageRect);

        worldBackgroundRenderSystem.render(entities);
        renderSystem.render(entities);

        // Render debug visuals if debugging.
        // Only render player focus if not debugging.
        if (isDebugging) {
            debugRenderSystem.render(entities);
        } else {
            playerFocusRenderSystem.render(entities);
        }

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
        // Clean out active collisions of all inactive entities. This must be flushed before destroying the entities themselves.
        // This might be the solution to the stale memory access crashes.
        std::erase_if(
            collisionSystem.activeCollisions,
            [](CollisionKey c) {
                return !c.first->isActive() || !c.second->isActive();
            }
        );

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

    AudioEventQueue& getAudioEventQueue() {
        return audioEventQueue;
    }

    Map& getMap() {
        return map;
    }

    std::function<void(Vector2D pos, Vector2D dir, float speed)> requestBulletSpawn;
};
