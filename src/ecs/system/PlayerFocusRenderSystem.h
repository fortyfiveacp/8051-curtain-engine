#pragma once
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"
#include "manager/TextureManager.h"

class PlayerFocusRenderSystem {
public:
    void render(const std::vector<std::unique_ptr<Entity>>& entities) {
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

        // If player or keyboard input not found, no rendering.
        if (playerEntity == nullptr || keyboardInput == nullptr) {
            return;
        }

        // If player is currently focusing, draw focus hitbox that is slightly larger than the actual hitbox.
        // Note that it's intended for turning on debug mode to not render the focus hitbox.
        if (keyboardInput->focus) {
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
};
