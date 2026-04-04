#include "PlayerAbilitySystem.h"
#include "Component.h"
#include "Entity.h"
#include "Game.h"
#include "manager/AudioManager.h"

void PlayerAbilitySystem::update(const std::vector<std::unique_ptr<Entity>>& entities) {
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

                playerStats.currentBombs--;
                Game::gameState.playerBombs = playerStats.currentBombs;

                // TODO: actual bomb logic.
            } else if (keyboardInput.bomb) {
                keyboardInput.bomb = false;
            }
        }
    }
}