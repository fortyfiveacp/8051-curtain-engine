#pragma once
#include <memory>

#include "Component.h"

class IconCounterSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities) {
        Entity* playerEntity = nullptr;

        // Find the player.
        for (auto& e : entities) {
            if (e->hasComponent<PlayerTag>()) {
                playerEntity = e.get();
                break;
            }
        }

        if (!playerEntity) {
            return;
        }

        auto& playerStats = playerEntity->getComponent<PlayerStats>();

        for (auto& e : entities) {
            if (e->hasComponent<IconCounter>()) {
                auto& iconLabel = e->getComponent<IconCounter>();

                // Update player icon labels if the current numbers have changed.
                if (iconLabel.type == IconCounterType::Health && playerStats.currentHealth != iconLabel.currentNumber) {
                    iconLabel.currentNumber = playerStats.currentHealth;
                    iconLabel.dirty = true;
                } else if (iconLabel.type == IconCounterType::Bomb && playerStats.currentBombs != iconLabel.currentNumber) {
                    iconLabel.currentNumber = playerStats.currentBombs;
                    iconLabel.dirty = true;
                }
            }
        }
    }
};
