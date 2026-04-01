#include "DebugRenderSystem.h"

#include "World.h"
#include "event/BaseEvent.h"

void DebugRenderSystem::update(World& world, const SDL_Event& event, bool isDebugging) {
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_TAB) {
        // Enable debugging via event.
        world.getEventManager().emit(DebugEvent{!isDebugging});
    }
}

void DebugRenderSystem::render(const std::vector<std::unique_ptr<Entity>>& entities) {
    // Draw all colliders.
    for (auto& entity : entities) {
        if (entity->hasComponent<RectCollider>()) {
            auto& c = entity->getComponent<RectCollider>();

            SDL_Texture* tex = TextureManager::load("../asset/tileset.png");
            SDL_FRect colSrc {32, 32, 32, 32};

            TextureManager::draw(tex, &colSrc, &c.rect);
        }

        if (entity->hasComponent<CircleCollider>()) {
            auto& c = entity->getComponent<CircleCollider>();

            SDL_Texture* tex = TextureManager::load("../asset/ball.png");
            SDL_FRect colSrc {0, 0, 32, 32};

            float colliderTopLeftX = c.centerPosition.x + c.offset.x - c.radius;
            float colliderTopLeftY = c.centerPosition.y + c.offset.y - c.radius;

            SDL_FRect colDst {colliderTopLeftX, colliderTopLeftY, c.radius, c.radius};

            TextureManager::draw(tex, &colSrc, &colDst);
        }
    }
}