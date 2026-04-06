#pragma once
#include <functional>

#include "event/BaseEvent.h"

class Entity;
class World;

class EventResponseSystem {
public:
    EventResponseSystem(World& world);

private:
    // Collisions
    void onCollision(const CollisionEvent& e, const char* otherTag, World& world);

    void onBombCollision(const CollisionEvent &e);

    bool getCollisionEntities(const CollisionEvent& e, const char* otherTag, Entity*& player, Entity*& other);

    // Player action
    void onPlayerAction(const PlayerActionEvent& e,
        const std::function<void(Entity* player, PlayerAction action)>& callback);

    void onUIInteraction(const UIInteractionEvent& e);
};