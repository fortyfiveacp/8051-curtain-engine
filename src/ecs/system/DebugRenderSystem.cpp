#include "DebugRenderSystem.h"

#include "World.h"
#include "event/BaseEvent.h"

void DebugRenderSystem::update(World& world, const SDL_Event& event, bool isDebugging) {
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_TAB) {
        // Toggle debugging via event.
        world.getEventManager().emit(DebugEvent{!isDebugging});
    }
}

void DebugRenderSystem::render(const std::vector<std::unique_ptr<Entity>>& entities) {
    // If debugging, draw all colliders with accurate sizing.
    for (auto& entity : entities) {
        if (entity->hasComponent<RectCollider>()) {
            auto& c = entity->getComponent<RectCollider>();

            SDL_Texture* tex = TextureManager::load("../asset/debug/tileset.png");
            SDL_FRect colSrc {32, 32, 32, 32};

            TextureManager::draw(tex, &colSrc, &c.rect);
        }
        if (entity->hasComponent<CircleCollider>()) {
            auto& c = entity->getComponent<CircleCollider>();

            SDL_Texture* tex = TextureManager::load("../asset/debug/debug-circle.png");
            SDL_FRect colSrc {0, 0, 32, 32};

            float colliderTopLeftX = c.centerPosition.x - c.radius;
            float colliderTopLeftY = c.centerPosition.y - c.radius;

            SDL_FRect colDst {colliderTopLeftX, colliderTopLeftY, c.radius * 2, c.radius * 2};

            TextureManager::draw(tex, &colSrc, &colDst);
        }
    }
}