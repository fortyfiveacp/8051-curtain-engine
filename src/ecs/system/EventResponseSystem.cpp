#include "EventResponseSystem.h"

#include "Game.h"
#include "World.h"

EventResponseSystem::EventResponseSystem(World &world) {
    // Subscriptions.
    world.getEventManager().subscribe([this, &world](const BaseEvent& e) {
        if (e.type != EventType::Collision) {
            return;
        }

        const auto& collision = static_cast<const CollisionEvent&>(e); // Cast base type to collision type.

        onCollision(collision, "item", world);
        onCollision(collision, "wall", world);
        onCollision(collision, "projectile", world);
    });

    world.getEventManager().subscribe([this, &world](const BaseEvent& e) {
        if (e.type != EventType::PlayerAction) {
            return;
        }

        const auto& playerAction = static_cast<const PlayerActionEvent&>(e);

        // TODO: onPlayerAction
    });

    world.getEventManager().subscribe([this, &world](const BaseEvent& e) {
        if (e.type != EventType::UIInteraction) {
            return;
        }

        const auto& uiInteractionEvent = static_cast<const UIInteractionEvent&>(e);
        onUIInteraction(uiInteractionEvent);
    });
}

void EventResponseSystem::onUIInteraction(const UIInteractionEvent& e) {
    if (!e.entity->hasComponent<SelectableUI>()) return;

    auto& selectable = e.entity->getComponent<SelectableUI>();

    switch (e.state) {
        case UIInteractionState::Pressed:
            selectable.onPressed();
            break;
        case UIInteractionState::Released:
            selectable.selected = false;
            selectable.onReleased();
            break;
        case UIInteractionState::Selected:
            selectable.selected = true;
            selectable.onSelect();
            break;
        default:
            break;
    }
}

void EventResponseSystem::onCollision(const CollisionEvent& e, const char* otherTag, World& world) {
    Entity* player = nullptr;
    Entity* other = nullptr;

    if (!getCollisionEntities(e, otherTag, player, other)) {
        return;
    }

    if (std::string(otherTag) == "item") {
        if (e.state != CollisionState::Enter) {
            return;
        }

        AudioManager::playSfx("item");

        auto& item = other->getComponent<Item>();

        if (player->hasComponent<PlayerStats>()) {
            auto& playerStats = player->getComponent<PlayerStats>();

            switch(item.type) {
                case Point:
                    playerStats.currentScore = std::min(playerStats.currentScore + item.value, PlayerStats::MAX_SCORE);
                    playerStats.currentPoint++;

                    // Also update current HiScore if current Score exceeds it.
                    playerStats.currentHiScore = std::max(playerStats.currentHiScore, playerStats.currentScore);
                    break;
                case SmallPower:
                case LargePower:
                    playerStats.currentPower = std::min(playerStats.currentPower + item.value, PlayerStats::MAX_POWER);
                    break;
                case Bomb:
                    playerStats.currentBombs = std::min(playerStats.currentBombs + item.value, PlayerStats::MAX_BOMBS);
                    break;
                default:
                    break;
            }
        }

        // TODO: purge.
        // for (auto& entity : world.getEntities()) {
        //     if (!entity->hasComponent<SceneState>()) {
        //         continue;
        //     }
        //
        //     Scene state
        //     auto& sceneState = entity->getComponent<SceneState>();
        //     sceneState.coinsCollected++;
        //
        //     if (sceneState.coinsCollected > 1) {
        //         Game::onSceneChangeRequest("level2");
        //     }
        // }

        other->destroy();
    }
    else if (std::string(otherTag) == "wall") {
        // Stop the player
        if (e.state != CollisionState::Stay) {
            return;
        }

        auto& t = player->getComponent<Transform>();
        t.position = t.oldPosition;
    }
    else if (std::string(otherTag) == "projectile") {
        if (e.state != CollisionState::Enter) {
            return;
        }

        if (player->hasComponent<InvincibilityFrames>()) {
            auto& invincibilityFrames = player->getComponent<InvincibilityFrames>();

            // Ignore hit if invincible.
            if (invincibilityFrames.active) {
                return;
            }

            // Enable invincibility after being hit.
            invincibilityFrames.active = true;
        }

        // Enable respawn when hit.
        if (player->hasComponent<PlayerRespawn>()) {
            auto& playerRespawn = player->getComponent<PlayerRespawn>();
            playerRespawn.isRespawning = true;
        }

        world.getAudioEventQueue().push(std::make_unique<AudioEvent>("player-hit"));

        // This logic is simple and direct.
        // Ideally, we would only operate on data in an update function (transient entities).
        auto& playerStats = player->getComponent<PlayerStats>();

        // Lose 70% of player power on hit.
        playerStats.currentPower *= 0.3;

        // Decrement player health.
        playerStats.currentHealth--;
        Game::gameState.playerHealth = playerStats.currentHealth;

        // If the player has no health left, bring up the continue game menu.
        if (playerStats.currentHealth == 0) {
            for (auto& entity : world.getEntities()) {
                if (entity->hasComponent<ContinueGameMenuTag>() && entity->hasComponent<Toggleable>()) {
                    entity->getComponent<Toggleable>().toggle();

                    break;
                }
            }
        }
    }
}

bool EventResponseSystem::getCollisionEntities(const CollisionEvent &e, const char *otherTag, Entity *&player,
Entity *&other) {
    if (e.entityA == nullptr || e.entityB == nullptr) {
        return false;
    }

    if (!(e.entityA->hasComponent<Collider>() && e.entityB->hasComponent<Collider>())) {
        return false;
    }

    auto& colliderA = e.entityA->getComponent<Collider>();
    auto& colliderB = e.entityB->getComponent<Collider>();

    if (colliderA.tag == "player" && colliderB.tag == otherTag) {
        player = e.entityA;
        other = e.entityB;
    }
    else if (colliderA.tag == otherTag && colliderB.tag == "player") {
        player = e.entityB;
        other = e.entityA;
    }

    return player && other;
}
