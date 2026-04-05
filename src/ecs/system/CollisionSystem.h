#pragma once
#include <algorithm>
#include <memory>
#include <set>
#include <vector>

#include "Entity.h"

using CollisionKey = std::pair<Entity*, Entity*>;

// Forward declaring.
class World;

class CollisionSystem {
public:
    void update(World& world);
    std::set<CollisionKey> activeCollisions;

private:
    std::pair<std::vector<Entity*>, std::vector<Entity*>> queryCollidables(const std::vector<std::unique_ptr<Entity> > &entities);

    void emitOnCollisionEvent(Entity& entityA, Entity& entityB, std::set<CollisionKey>& collisions, World& world);

    CollisionKey makeKey(Entity* entityA, Entity* entityB) {
        return std::minmax(entityA, entityB); // minmax automatically orders our pair, smaller number (memory address) is ordered first
    }
};
