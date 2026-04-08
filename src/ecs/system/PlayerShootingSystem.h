#pragma once
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"


class PlayerShootingSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities) {
        for (auto& entity :entities) {
            if (entity->hasComponent<PlayerTag>() && entity->hasComponent<KeyboardInput>()
                && entity->hasComponent<PlayerStats>() && entity->hasComponent<Children>()) {
                auto& stats = entity->getComponent<PlayerStats>();
                auto& keyboardInput = entity->getComponent<KeyboardInput>();
                auto& children = entity->getComponent<Children>();

                bool isRespawning = false;

                // Check if tf the player is respawning.
                if (entity->hasComponent<PlayerRespawn>() && entity->getComponent<PlayerRespawn>().isRespawning) {
                    isRespawning = true;
                }

                for (auto& child : children.children) {
                    if (child->hasComponent<LinearSpawner>()) {
                        auto& linearSpawner = child->getComponent<LinearSpawner>();

                        // If the player is holding down shoot and not respawning, enable shooting.
                        linearSpawner.isActive = keyboardInput.shoot && !isRespawning;
                    }
                }
            }
        }
    }
};
