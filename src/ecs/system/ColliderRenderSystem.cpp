#include "ColliderRenderSystem.h"

#include "World.h"
#include "event/BaseEvent.h"

void ColliderRenderSystem::update(World& world, const SDL_Event& event, bool isDebugging) {
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_TAB) {
        // Toggle debugging via event.
        world.getEventManager().emit(DebugEvent{!isDebugging});
    }
}

void ColliderRenderSystem::render(const std::vector<std::unique_ptr<Entity>>& entities, bool isDebugging) {
    Entity* playerEntity = nullptr;
    KeyboardInput* keyboardInput = nullptr;

    // Find player entity and keyboard input. Note that only circle colliders for the player are supported.
    for (auto& entity : entities) {
        if (entity->hasComponent<KeyboardInput>() && entity->hasComponent<CircleCollider>() &&
            entity->hasComponent<PlayerTag>()) {
            playerEntity = entity.get();
            keyboardInput = &playerEntity->getComponent<KeyboardInput>();
        }
    }

    // If debugging, draw all colliders with accurate sizing.
    if (isDebugging) {
        for (auto& entity : entities) {
            if (entity->hasComponent<RectCollider>()) {
                auto& c = entity->getComponent<RectCollider>();

                SDL_Texture* tex = TextureManager::load("../asset/tileset.png");
                SDL_FRect colSrc {32, 32, 32, 32};

                TextureManager::draw(tex, &colSrc, &c.rect);
            }
            if (entity->hasComponent<CircleCollider>()) {
                auto& c = entity->getComponent<CircleCollider>();

                SDL_Texture* tex = TextureManager::load("../asset/ui/debug-circle.png");
                SDL_FRect colSrc {0, 0, 32, 32};

                float colliderTopLeftX = c.centerPosition.x - c.radius;
                float colliderTopLeftY = c.centerPosition.y - c.radius;

                SDL_FRect colDst {colliderTopLeftX, colliderTopLeftY, c.radius * 2, c.radius * 2};

                TextureManager::draw(tex, &colSrc, &colDst);
            }
        }
    } else if (playerEntity && keyboardInput && keyboardInput->focus) {
        // If player entity was found, and is currently focusing, draw focus hitbox that is slightly larger
        // than the actual hitbox. Note that it's intended for turning on debug mode to not render the focus hitbox.
        auto& c = playerEntity->getComponent<CircleCollider>();
        SDL_Texture* tex = TextureManager::load("../asset/ui/circle-collider.png");
        SDL_FRect colSrc {0, 0, 14, 14};

        float sizeIncreaseFactor = 1.7f;
        float radiusIncreaseFactor = 2.0f * sizeIncreaseFactor;

        float colliderTopLeftX = c.centerPosition.x - c.radius * sizeIncreaseFactor;
        float colliderTopLeftY = c.centerPosition.y - c.radius * sizeIncreaseFactor;

        SDL_FRect colDst {colliderTopLeftX, colliderTopLeftY, c.radius * radiusIncreaseFactor, c.radius * radiusIncreaseFactor};

        TextureManager::draw(tex, &colSrc, &colDst);
    }
}