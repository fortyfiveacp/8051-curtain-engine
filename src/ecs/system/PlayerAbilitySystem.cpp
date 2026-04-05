#include "PlayerAbilitySystem.h"

#include "PlayerBombFactory.h"
#include "Component.h"
#include "Entity.h"
#include "Game.h"
#include "manager/AudioManager.h"

void PlayerAbilitySystem::update(World& world, float deltaTime) {
    for (auto& entity : world.getEntities()) {
        if (entity->hasComponent<PlayerTag>() && entity->hasComponent<KeyboardInput>() && entity->hasComponent<PlayerStats>()) {
            auto& keyboardInput = entity->getComponent<KeyboardInput>();
            auto& playerStats = entity->getComponent<PlayerStats>();
            const auto& transform = entity->getComponent<Transform>();

            if (keyboardInput.shoot) {
                // TODO: shooting logic.
            }

            // TODO: Introduce bomb cooldown
            if (keyboardInput.bomb && playerStats.currentBombs > 0) {
                AudioManager::playSfx("bomb");
                keyboardInput.bomb = false;

                playerStats.currentBombs--;
                Game::gameState.playerBombs = playerStats.currentBombs;

                castBomb(*entity, transform, world);

            } else if (keyboardInput.bomb) {
                keyboardInput.bomb = false;
            }
        }
    }
}

void PlayerAbilitySystem::castBomb(Entity& entity, const Transform& transform, World& world) {
    float bombSize = 500.0f;
    float bombX = transform.position.x - (bombSize / 2.0f);
    float bombY = transform.position.y - (bombSize / 2.0f);

    PlayerBombFactory::buildBasicBomb(world, Vector2D(bombX, bombY), bombSize);

    if (entity.hasComponent<InvincibilityFrames>()) {
        auto& iFrames = entity.getComponent<InvincibilityFrames>();
        iFrames.active = true;
        iFrames.timer = 0.0f;
    }
}