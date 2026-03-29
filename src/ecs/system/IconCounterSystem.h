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
                auto& iconCounter = e->getComponent<IconCounter>();

                // Update icon counter if the current numbers have changed.
                if (iconCounter.type == IconCounterType::Health && playerStats.currentHealth != iconCounter.currentNumber) {
                    iconCounter.currentNumber = playerStats.currentHealth;
                    iconCounter.dirty = true;
                } else if (iconCounter.type == IconCounterType::Bomb && playerStats.currentBombs != iconCounter.currentNumber) {
                    iconCounter.currentNumber = playerStats.currentBombs;
                    iconCounter.dirty = true;
                }
            }
        }
    }
};
