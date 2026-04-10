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

    void onPlayerShotCollision(const CollisionEvent& e);

    void onBombCollision(const CollisionEvent &e, World& world);

    bool getCollisionEntities(const CollisionEvent& e, const char* otherTag, Entity*& player, Entity*& other);

    void onUIInteraction(const UIInteractionEvent& e);
};