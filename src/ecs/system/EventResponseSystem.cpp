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

        other->destroy();

        for (auto& entity : world.getEntities()) {
            if (!entity->hasComponent<SceneState>()) {
                continue;
            }

            // Scene state
            auto& sceneState = entity->getComponent<SceneState>();
            sceneState.coinsCollected++;

            if (sceneState.coinsCollected > 1) {
                Game::onSceneChangeRequest("level2");
            }
        }
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

        // This logic is simple and direct.
        // Ideally, we would only operate on data in an update function (transient entities).
        auto& health = player->getComponent<Health>();
        health.currentHealth--;

        Game::gameState.playerHealth = health.currentHealth;

        std::cout << health.currentHealth << std::endl;

        if (health.currentHealth == 0) {
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
