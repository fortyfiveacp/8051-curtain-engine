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

        onBombCollision(collision);
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
                    Game::gameState.score = playerStats.currentScore;

                    playerStats.currentPoint++;
                    Game::gameState.point = playerStats.currentPoint;

                    // Also update current HiScore if current Score exceeds it.
                    playerStats.currentHiScore = std::max(playerStats.currentHiScore, playerStats.currentScore);
                    Game::gameState.hiScore = playerStats.currentHiScore;
                    break;
                case SmallPower:
                case LargePower:
                    playerStats.currentPower = std::min(playerStats.currentPower + item.value, PlayerStats::MAX_POWER);
                    Game::gameState.power = playerStats.currentPower;
                    break;
                case Bomb:
                    playerStats.currentBombs = std::min(playerStats.currentBombs + item.value, PlayerStats::MAX_BOMBS);
                    Game::gameState.playerBombs = playerStats.currentBombs;
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

// TODO: Decide whether to make onCollision accept any two entities and merge this function's logic into it.
void EventResponseSystem::onBombCollision(const CollisionEvent& e) {
    if (e.entityA == nullptr || e.entityB == nullptr) return;

    // Guard: Ensure both entities actually have CircleColliders before accessing them
    if (!e.entityA->hasComponent<CircleCollider>() || !e.entityB->hasComponent<CircleCollider>()) {
        return;
    }

    Entity* bomb = nullptr;
    Entity* other = nullptr;

    auto& colA = e.entityA->getComponent<CircleCollider>();
    auto& colB = e.entityB->getComponent<CircleCollider>();

    if (colA.tag == "bomb") {
        bomb = e.entityA;
        other = e.entityB;
    } else if (colB.tag == "bomb") {
        bomb = e.entityB;
        other = e.entityA;
    }

    if (!bomb || !other) return;

    // Now it is safe to check the tag of the "other" entity
    auto& otherCol = other->getComponent<CircleCollider>();

    if (otherCol.tag == "projectile") {
        other->destroy();
    }

    if (otherCol.tag == "enemy" || otherCol.tag == "boss") {
        // TODO: Have enemy/boss entities take damage while they are inside the collider.
    }
}

bool EventResponseSystem::getCollisionEntities(const CollisionEvent &e, const char *otherTag, Entity *&player,
Entity *&other) {
    if (e.entityA == nullptr || e.entityB == nullptr) {
        return false;
    }

    bool entityAHasRect = e.entityA->hasComponent<RectCollider>();
    bool entityBHasRect = e.entityB->hasComponent<RectCollider>();
    bool entityAHasCircle = e.entityA->hasComponent<CircleCollider>();
    bool entityBHasCircle = e.entityB->hasComponent<CircleCollider>();

    if (!((entityAHasRect || entityAHasCircle) && (entityBHasRect || entityBHasCircle))) {
        return false;
    }

    const std::string* tagA = nullptr;
    const std::string* tagB = nullptr;

    if (entityAHasRect) {
        tagA = &e.entityA->getComponent<RectCollider>().tag;
    }
    else {
        tagA = &e.entityA->getComponent<CircleCollider>().tag;
    }

    if (entityBHasRect) {
        tagB = &e.entityB->getComponent<RectCollider>().tag;
    }
    else {
        tagB = &e.entityB->getComponent<CircleCollider>().tag;
    }

    if (*tagA == "player" && *tagB == otherTag) {
        player = e.entityA;
        other = e.entityB;
    }
    else if (*tagA == otherTag && *tagB == "player") {
        player = e.entityB;
        other = e.entityA;
    }

    return player && other;
}
