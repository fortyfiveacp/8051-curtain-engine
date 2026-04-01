#pragma once

#include "DebugRenderSystem.h"

#include "World.h"
#include "event/BaseEvent.h"

void DebugRenderSystem::update(World& world, const SDL_Event& event, bool isDebugging) {
    // Bring up pause menu and pause the game with the escape key.
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_TAB) {
        world.getEventManager().emit(DebugEvent{!isDebugging});
    }
}

void DebugRenderSystem::render(const std::vector<std::unique_ptr<Entity>>& entities) {
    // Draw all colliders.
    for (auto& entity : entities) {
        if (entity->hasComponent<Collider>()) {
            auto& c = entity->getComponent<Collider>();

            SDL_Texture* tex = TextureManager::load("../asset/tileset.png");
            SDL_FRect colSrc {32, 32, 32, 32};

            TextureManager::draw(tex, &colSrc, &c.rect);
        }
    }
}