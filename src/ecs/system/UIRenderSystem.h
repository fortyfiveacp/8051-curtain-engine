#pragma once
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"
#include "RenderUtils.h"
#include "manager/TextureManager.h"

class UIRenderSystem {
public:
    void render(const std::vector<std::unique_ptr<Entity>>& entities) {
        for (auto& entity : entities) {
            if (entity->hasComponent<Transform>() && entity->hasComponent<Sprite>()) {
                auto transform = entity->getComponent<Transform>();
                auto sprite = entity->getComponent<Sprite>();

                if (sprite.renderLayer != RenderLayer::UI) {
                    continue;
                }

                // No converting from world space to screen space for UI.
                sprite.dst.x = transform.position.x;
                sprite.dst.y = transform.position.y;

                if (sprite.visible) {
                    SDL_FRect scaledDest = RenderUtils::getScaledDest(sprite.dst, transform.scale);
                    TextureManager::draw(sprite.texture, sprite.src, scaledDest);
                }
            }
        }
    }
};
