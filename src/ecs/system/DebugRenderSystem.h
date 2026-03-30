#pragma once
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"
#include "../../manager/TextureManager.h"

class DebugRenderSystem {
public:
    void render(std::vector<std::unique_ptr<Entity>>& entities) {
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
};