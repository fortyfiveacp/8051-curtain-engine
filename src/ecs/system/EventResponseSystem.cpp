#include "EventResponseSystem.h"

#include "Game.h"
#include "ItemFactory.h"
#include "ItemUtils.h"
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
        onCollision(collision, "enemy", world);

        onBombCollision(collision, world);
        onPlayerShotCollision(collision);
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
                    // Only actual point items count towards the point counter.
                    playerStats.currentPoint++;
                    Game::gameState.point = playerStats.currentPoint;
                case Star:
                    playerStats.currentScore = std::min(playerStats.currentScore + item.value, PlayerStats::MAX_SCORE);
                    Game::gameState.score = playerStats.currentScore;

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
    else if (std::string(otherTag) == "projectile" || std::string(otherTag) == "enemy") {
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

        if (player->hasComponent<DeathBombState>()) {
            auto& deathBombState = player->getComponent<DeathBombState>();
            if (!deathBombState.isHit) {
                deathBombState.isHit = true;
                deathBombState.timer = 0.0f;

                world.getAudioEventQueue().push(std::make_unique<AudioEvent>("player-hit"));
            }
        }
    }
}

void EventResponseSystem::onPlayerShotCollision(const CollisionEvent& e) {
    if (e.entityA == nullptr || e.entityB == nullptr) {
        return;
    }

    Entity* playerShotEntity = nullptr;
    Entity* other = nullptr;

    if (e.entityA->hasComponent<CircleCollider>() && e.entityA->getComponent<CircleCollider>().tag == "player-shot") {
        playerShotEntity = e.entityA;
        other = e.entityB;
    } else if (e.entityB->hasComponent<CircleCollider>() && e.entityB->getComponent<CircleCollider>().tag == "player-shot") {
        playerShotEntity = e.entityB;
        other = e.entityA;
    }

    if (playerShotEntity && other) {
        std::string otherTag{};

        if (other->hasComponent<RectCollider>()) {
            otherTag = other->getComponent<RectCollider>().tag;
        } else if (other->hasComponent<CircleCollider>()) {
            otherTag = other->getComponent<CircleCollider>().tag;
        }

        if (otherTag == "enemy" && other->hasComponent<EnemyHealth>()) {
            if (e.state == CollisionState::Enter) {
                auto& health = other->getComponent<EnemyHealth>();
                auto& playerShot = playerShotEntity->getComponent<PlayerShot>();

                health.current -= static_cast<int>(playerShot.damage);
            }

            // Destroy shot after impact.
            playerShotEntity->destroy();
        }

        if (otherTag == "boss" && other->hasComponent<Boss>()) {
            if (e.state == CollisionState::Enter) {
                auto& boss = other->getComponent<Boss>();
                auto& playerShot = playerShotEntity->getComponent<PlayerShot>();
                if (!boss.isInvulnerable) {
                    boss.currentHealth -= static_cast<int>(playerShot.damage);
                }
            }

            // Destroy shot after impact.
            playerShotEntity->destroy();
        }
    }
}

void EventResponseSystem::onBombCollision(const CollisionEvent& e, World& world) {
    if (e.entityA == nullptr || e.entityB == nullptr) {
        return;
    }

    Entity* bomb = nullptr;
    Entity* other = nullptr;

    if (e.entityA->hasComponent<CircleCollider>() && e.entityA->getComponent<CircleCollider>().tag == "bomb") {
        bomb = e.entityA;
        other = e.entityB;
    } else if (e.entityB->hasComponent<CircleCollider>() && e.entityB->getComponent<CircleCollider>().tag == "bomb") {
        bomb = e.entityB;
        other = e.entityA;
    }

    if (bomb && other) {
        std::string otherTag{};
        if (other->hasComponent<RectCollider>()) {
            otherTag = other->getComponent<RectCollider>().tag;
        } else if (other->hasComponent<CircleCollider>()) {
            otherTag = other->getComponent<CircleCollider>().tag;
        }

        if (otherTag == "projectile") {
            if (other->hasComponent<Transform>()) {
                auto& itemEntity = world.createDeferredEntity();
                ItemFactory::createItem(itemEntity, Star, other->getComponent<Transform>().position);
            }

            other->destroy();
        }

        if (otherTag == "enemy" || otherTag == "boss") {
            if (e.state == CollisionState::Exit) {
                return;
            }

            float bombDamage = bomb->getComponent<PlayerBomb>().damage;
            if (other->hasComponent<EnemyHealth>()) {
                other->getComponent<EnemyHealth>().current -= static_cast<int>(bombDamage);
            } else if (other->hasComponent<Boss>()) {
                auto& boss = other->getComponent<Boss>();
                if (!boss.isInvulnerable) {
                    boss.currentHealth -= static_cast<int>(bombDamage * 5);
                }
            }
        }
    }
}

// TODO: Generalize implementation so onBombCollision implementation can be simplified.
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
