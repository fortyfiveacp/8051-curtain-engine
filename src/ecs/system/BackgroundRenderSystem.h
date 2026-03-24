#pragma once
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"
#include "RenderUtils.h"
#include "manager/TextureManager.h"

class BackgroundRenderSystem {
public:
    void render(std::vector<std::unique_ptr<Entity>>& entities) {
        for (auto& entity : entities) {
            if (entity->hasComponent<Transform>()) {
                auto transform = entity->getComponent<Transform>();

                if (entity->hasComponent<Sprite>()) {
                    auto sprite = entity->getComponent<Sprite>();

                    // Only render sprites on the Background layer.
                    if (sprite.renderLayer != RenderLayer::Background) {
                        continue;
                    }

                    // No converting from world space to screen space for Background.
                    sprite.dst.x = transform.position.x;
                    sprite.dst.y = transform.position.y;

                    if (sprite.visible) {
                        SDL_FRect scaledDest = RenderUtils::getScaledDest(sprite.dst, transform.scale);
                        TextureManager::draw(sprite.texture, &sprite.src, &scaledDest);
                    }
                } else if (entity->hasComponent<Label>()) {
                    auto label = entity->getComponent<Label>();

                    label.dst.x = transform.position.x;
                    label.dst.y = transform.position.y;

                    if (label.visible) {
                        SDL_FRect scaledDest = RenderUtils::getScaledDest(label.dst, transform.scale);
                        TextureManager::draw(label.texture, nullptr, &scaledDest);
                    }
                }
            }
        }
    }
};
