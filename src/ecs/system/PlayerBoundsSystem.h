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

            // The additional distance the player is allowed to move out of bounds.
            float boundsAllowanceX = sprite.dst.w / 3;
            float boundsAllowanceY = sprite.dst.h / 3;

            // If the player comes into contact with an edge, disable their movement in the appropriate direction.
            // Right edge of view.
            if (playerTransform.position.x > (cam.view.x + cam.view.w - sprite.dst.w) + boundsAllowanceX) {
                keyboardInput.right = false;
            }

            // Left edge of view.
            if (playerTransform.position.x < cam.view.x - boundsAllowanceX) {
                keyboardInput.left = false;
            }

            // Bottom edge of view.
            if (playerTransform.position.y > cam.view.y + cam.view.h - sprite.dst.h) {
                keyboardInput.down = false;
            }

            // Top edge of view.
            if (playerTransform.position.y < cam.view.y - boundsAllowanceY) {
                keyboardInput.up = false;
            }
        }
    }
}
