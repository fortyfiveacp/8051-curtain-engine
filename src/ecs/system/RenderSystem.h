#pragma once
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"
#include "../../manager/TextureManager.h"

class RenderSystem {
public:
    void render(std::vector<std::unique_ptr<Entity>>& entities) {
        Entity* cameraEntity = nullptr;

        // Find camera
        for (auto& e : entities) {
            if (e->hasComponent<Camera>()) {
                cameraEntity = e.get();
                break;
            }
        }

        // No camera = no rendering
        if (!cameraEntity) {
            return;
        }

        auto& cam = cameraEntity->getComponent<Camera>();

        for (auto& entity : entities) {
            if (entity->hasComponent<Transform>() && entity->hasComponent<Sprite>()) {
                auto& t = entity->getComponent<Transform>();
                auto& sprite = entity->getComponent<Sprite>();

                if (sprite.renderLayer != RenderLayer::World) {
                    continue;
                }

                // We are converting from world space to screen space.
                sprite.dst.x = t.position.x - cam.view.x;
                sprite.dst.y = t.position.y - cam.view.y;

                // If the entity has animation, update the source rect.
                if (entity->hasComponent<Animation>()) {
                    auto& anim = entity->getComponent<Animation>();
                    sprite.src = anim.clips[anim.currentClip].frameIndices[anim.currentFrame];
                }

                TextureManager::draw(sprite.texture, sprite.src, sprite.dst);
            }
        }
    }
};
