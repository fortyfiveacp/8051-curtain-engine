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

        const auto& keyboardInteractionEvent = static_cast<const UIInteractionEvent&>(e);
        onUIInteraction(keyboardInteractionEvent);
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
            selectable.onReleased();
            break;
        case UIInteractionState::Selected:
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

        auto& item = other->getComponent<Item>();

        for (auto& entity : world.getEntities()) {
            if (entity->hasComponent<PlayerStats>()) {
                auto& playerStats = entity->getComponent<PlayerStats>();

                switch(item.type) {
                    case Point:
                        playerStats.currentScore += item.value;
                        playerStats.currentPoint++;
                        if (playerStats.currentScore > playerStats.currentHiScore) {
                            playerStats.currentHiScore = playerStats.currentScore;
                        }
                        break;
                    case SmallPower:
                    case LargePower:
                        playerStats.currentPower += item.value;
                        break;
                    case Bomb:
                        if (playerStats.currentBombs + item.value <= playerStats.maxBombs) {
                            playerStats.currentBombs += item.value;
                        }
                        break;
                    default:
                        break;
                }

                break;
            }
            // if (!entity->hasComponent<SceneState>()) {
            //     continue;
            // }

            // Scene state
            // auto& sceneState = entity->getComponent<SceneState>();
            // sceneState.coinsCollected++;
            //
            // if (sceneState.coinsCollected > 1) {
            //     Game::onSceneChangeRequest("level2");
            // }
        }

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

        auto& invincibilityFrames = player->getComponent<InvincibilityFrames>();
        auto& playerTransform = player->getComponent<Transform>();

        // Ignore hit if invincible.
        if (invincibilityFrames.active) {
            return;
        }

        // Enable invincibility frames after hit.
        invincibilityFrames.active = true;

        world.getAudioEventQueue().push(std::make_unique<AudioEvent>("player-hit"));

        // This logic is simple and direct.
        // Ideally, we would only operate on data in an update function (transient entities).
        auto& playerStats = player->getComponent<PlayerStats>();

        // Teleport player back to starting position.
        playerTransform.position = playerStats.playerStartingPosition;

        playerStats.currentHealth--;
        Game::gameState.playerHealth = playerStats.currentHealth;
        std::cout << playerStats.currentHealth << std::endl;

        if (playerStats.currentHealth == 0) {
            player->destroy();

            // Change scene (deferred).
            Game::onSceneChangeRequest("gameover");
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
