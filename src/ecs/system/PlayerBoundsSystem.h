#pragma once
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class PlayerBoundsSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities);
};

inline void PlayerBoundsSystem::update(const std::vector<std::unique_ptr<Entity>> &entities) {
    Entity* cameraEntity = nullptr;

    // Find camera entity.
    for (auto& entity : entities) {
        if (entity->hasComponent<Camera>()) {
            cameraEntity = entity.get();
            break;
        }
    }

    // If no camera then there are no bounds.
    if (!cameraEntity) {
        return;
    }

    auto& cam = cameraEntity->getComponent<Camera>();

    for (auto& entity : entities) {
        if (entity->hasComponent<PlayerTag>() && entity->hasComponent<Transform>() &&
            entity->hasComponent<KeyboardInput>() && entity->hasComponent<Sprite>()) {
            auto& playerTransform = entity->getComponent<Transform>();
            auto& sprite = entity->getComponent<Sprite>();
            auto& keyboardInput = entity->getComponent<KeyboardInput>();

            // Additional padding on the screen boundary that prevents the player from moving their collider to
            // the very edge of the screen.
            float boundsPaddingX = sprite.dst.w / 4.25f;
            float boundsPaddingY = sprite.dst.h / 4.25f;

            // If the player comes reaches an edge, disable their movement in the appropriate direction.
            // Right edge of view.
            if (playerTransform.position.x >= (cam.view.x + cam.view.w) - boundsPaddingX) {
                keyboardInput.right = false;
            }

            // Left edge of view.
            if (playerTransform.position.x <= cam.view.x + boundsPaddingX) {
                keyboardInput.left = false;
            }

            // Bottom edge of view.
            if (playerTransform.position.y >= cam.view.y + cam.view.h - boundsPaddingY) {
                keyboardInput.down = false;
            }

            // Top edge of view.
            if (playerTransform.position.y <= cam.view.y + boundsPaddingY) {
                keyboardInput.up = false;
            }
        }
    }
}
