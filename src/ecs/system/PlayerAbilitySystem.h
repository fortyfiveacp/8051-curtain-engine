#pragma once
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"
#include "manager/AudioManager.h"

class PlayerAbilitySystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities) {
        for (auto& entity : entities) {
            if (entity->hasComponent<PlayerTag>() && entity->hasComponent<KeyboardInput>() && entity->hasComponent<PlayerStats>()) {
                auto& keyboardInput = entity->getComponent<KeyboardInput>();
                auto& playerStats = entity->getComponent<PlayerStats>();

                if (keyboardInput.shoot) {
                    // TODO: shooting logic.
                }

                if (keyboardInput.bomb && playerStats.currentBombs > 0) {
                    AudioManager::playSfx("bomb");
                    keyboardInput.bomb = false;
                    playerStats.currentBombs--; // TODO: need to update game state counter too.

                    // TODO: actual bomb logic.
                } else {
                    keyboardInput.bomb = false;
                }
            }
        }
    }
};
