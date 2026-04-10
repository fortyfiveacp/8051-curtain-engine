#include "PlayerBombAbilitySystem.h"

#include "PlayerBombFactory.h"
#include "Component.h"
#include "Entity.h"
#include "Game.h"
#include "manager/AudioManager.h"

void PlayerBombAbilitySystem::update(World& world, float deltaTime) {
    for (auto& entity : world.getEntities()) {
        if (entity->hasComponent<PlayerTag>() && entity->hasComponent<KeyboardInput>()
            && entity->hasComponent<PlayerStats>() && entity->hasComponent<PlayerBombAbility>()) {
            auto& bomb = entity->getComponent<PlayerBombAbility>();
            auto& stats = entity->getComponent<PlayerStats>();
            auto& keyboardInput = entity->getComponent<KeyboardInput>();

            if (bomb.active) {
                bomb.cooldownTimer -= deltaTime;
                if (bomb.cooldownTimer <= 0.0f) {
                    bomb.active = false;
                    bomb.cooldownTimer = 0.0f;
                }
            }

            bool isRespawning = entity->hasComponent<PlayerRespawn>() && entity->getComponent<PlayerRespawn>().isRespawning;
            bool isDeathBombing = entity->hasComponent<DeathBombState>() && entity->getComponent<DeathBombState>().isHit;

            if (keyboardInput.bomb) {
                bool canCast = !bomb.active && !isRespawning;
                if (stats.currentBombs > 0 && canCast) {
                    stats.currentBombs--;
                    Game::gameState.playerBombs = stats.currentBombs;

                    bomb.active = true;
                    bomb.cooldownTimer = bomb.cooldown;

                    if (isDeathBombing) {
                        entity->getComponent<DeathBombState>().isHit = false;
                        entity->getComponent<DeathBombState>().timer = 0.0f;
                    }

                    AudioManager::playSfx("bomb");
                    castBomb(*entity, entity->getComponent<Transform>(), world);
                }

                keyboardInput.bomb = false;
            }
        }
    }
}

void PlayerBombAbilitySystem::castBomb(Entity& entity, const Transform& transform, World& world) {
    float bombSize = 600.0f;
    float bombX = transform.position.x - (bombSize / 2.0f);
    float bombY = transform.position.y - (bombSize / 2.0f);

    PlayerBombFactory::buildBasicBomb(world, Vector2D(bombX, bombY), bombSize);

    if (entity.hasComponent<InvincibilityFrames>()) {
        auto& iFrames = entity.getComponent<InvincibilityFrames>();
        iFrames.active = true;
        iFrames.timer = 0.0f;
    }
}