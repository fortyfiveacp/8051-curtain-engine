#include "DeathBombSystem.h"
#include "Component.h"
#include "Game.h"

void DeathBombSystem::update(std::vector<std::unique_ptr<Entity>>& entities, float deltaTime) {
    for (auto& entity : entities) {
        if (entity->hasComponent<DeathBombState>() && entity->hasComponent<PlayerStats>()) {
            auto& deathBomb = entity->getComponent<DeathBombState>();

            if (!deathBomb.isHit) {
                continue;
            }

            deathBomb.timer += deltaTime;

            if (deathBomb.timer >= deathBomb.windowDuration) {
                deathBomb.isHit = false;

                // This logic is simple and direct.
                // Ideally, we would only operate on data in an update function (transient entities).
                auto& playerStats = entity->getComponent<PlayerStats>();

                if (entity->hasComponent<InvincibilityFrames>()) {
                    entity->getComponent<InvincibilityFrames>().active = true;
                }

                // Enable respawn when hit.
                if (entity->hasComponent<PlayerRespawn>()) {
                    entity->getComponent<PlayerRespawn>().isRespawning = true;
                }

                // Lose 70% of player power on hit.
                // playerStats.currentPower *= 0.3;

                // Decrement player health.
                playerStats.currentHealth--;
                Game::gameState.playerHealth = playerStats.currentHealth;

                // If the player has no health left, bring up the continue game menu.
                if (playerStats.currentHealth <= 0) {
                    for (auto& menuEntity : entities) {
                        if (menuEntity->hasComponent<ContinueGameMenuTag>() && menuEntity->hasComponent<Toggleable>()) {
                            menuEntity->getComponent<Toggleable>().toggle();
                            break;
                        }
                    }
                }
            }
        }
    }
}