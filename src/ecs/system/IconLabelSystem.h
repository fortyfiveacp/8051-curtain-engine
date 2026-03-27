#pragma once
#include <memory>

#include "Component.h"

class IconLabelSystem {
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

        auto& playerHealth = playerEntity->getComponent<Health>();
        auto& playerBombs = playerEntity->getComponent<Bombs>();

        for (auto& e : entities) {
            if (e->hasComponent<IconLabel>()) {
                auto& iconLabel = e->getComponent<IconLabel>();

                // Update player icon labels if the current numbers have changed.
                if (iconLabel.type == IconLabelType::Health && playerHealth.currentHealth != iconLabel.currentNumber) {
                    iconLabel.currentNumber = playerHealth.currentHealth;
                    iconLabel.dirty = true;
                } else if (iconLabel.type == IconLabelType::Bomb && playerBombs.currentBombs != iconLabel.currentNumber) {
                    iconLabel.currentNumber = playerBombs.currentBombs;
                    iconLabel.dirty = true;
                }
            }
        }
    }
};
