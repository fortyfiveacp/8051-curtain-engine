#pragma once
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"
#include "TransformUtils.h"

class PlayerRespawnSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, float dt) {
        for (auto& entity : entities) {
            if (entity->hasComponent<PlayerTag>() && entity->hasComponent<PlayerRespawn>() &&
                entity->hasComponent<Transform>()) {
                auto& playerRespawn = entity->getComponent<PlayerRespawn>();

                if (playerRespawn.isRespawning) {
                    auto& playerTransform = entity->getComponent<Transform>();

                    // Hide player off-screen.
                    TransformUtils::setPosition(entity, playerRespawn.playerStartingPosition + Vector2D(0, 150));

                    playerRespawn.timer += dt;

                    if (playerRespawn.timer >= playerRespawn.respawnDelay) {
                        // Disable respawn.
                        playerRespawn.timer = 0.0f;
                        playerRespawn.isRespawning = false;

                        // Reposition player at starting position.
                        TransformUtils::setPosition(entity, playerRespawn.playerStartingPosition);
                    }
                }

                // Should only be one player, so return early after operating on it.
                return;
            }
        }
    }
};
