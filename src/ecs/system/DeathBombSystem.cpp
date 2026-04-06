#include "DeathBombSystem.h"
#include "Component.h"
#include "Game.h"

void DeathBombSystem::update(World& world, float deltaTime) {
    for (auto& entity : world.getEntities()) {
        if (entity->hasComponent<DeathBombState>() && entity->hasComponent<PlayerStats>()) {
            auto& deathBomb = entity->getComponent<DeathBombState>();

            if (!deathBomb.isHit) {
                continue;
            }

            deathBomb.timer += deltaTime;

            if (deathBomb.timer >= deathBomb.windowDuration) {
                deathBomb.isHit = false;
                auto& playerStats = entity->getComponent<PlayerStats>();

                if (entity->hasComponent<InvincibilityFrames>()) {
                    entity->getComponent<InvincibilityFrames>().active = true;
                }
                if (entity->hasComponent<PlayerRespawn>()) {
                    entity->getComponent<PlayerRespawn>().isRespawning = true;
                }

                // playerStats.currentPower *= 0.3;
                playerStats.currentHealth--;
                Game::gameState.playerHealth = playerStats.currentHealth;

                if (playerStats.currentHealth <= 0) {
                    for (auto& menuEntity : world.getEntities()) {
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