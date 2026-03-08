#include "CollisionSystem.h"

#include <iostream>

#include "Collision.h"
#include "World.h"

// Has two important functions but they are related
// 1. Positions the collider with the transform
// 2. Checking for collisions

// This can be improved (e.g. for bullet hell)
void CollisionSystem::update(World& world) {
    // Get a list of entities that have colliders and transforms
    const std::vector<Entity*> collidables = queryCollidables(world.getEntities());

    // Update all collider positions first
    for (auto entity : collidables) {
        auto& t = entity->getComponent<Transform>();
        auto& c = entity->getComponent<Collider>();
        c.rect.x = t.position.x;
        c.rect.y = t.position.y;
    }

    std::set<CollisionKey> currentCollisions;

    // Outer loop
    for (size_t i = 0; i < collidables.size(); i++) {
        // Update the collider position
        auto entityA = collidables[i];
        auto& t = entityA->getComponent<Transform>();
        auto& colliderA = entityA->getComponent<Collider>();

        colliderA.rect.x = t.position.x;
        colliderA.rect.y = t.position.y;

        // Check for collider collision
        // Inner loop
        for (size_t j = i + 1; j < collidables.size(); j++) {
            auto entityB = collidables[j];
            auto& colliderB = entityB->getComponent<Collider>();

            if (Collision::AABB(colliderA, colliderB)) {
                // std::cout << colliderA.tag << " hit " << colliderB.tag << std::endl;
                CollisionKey key = makeKey(entityA, entityB);
                currentCollisions.insert(key);
                if (!activeCollisions.contains(key)) {
                    world.getEventManager().emit(CollisionEvent{entityA, entityB, CollisionState::Enter});
                }
                world.getEventManager().emit(CollisionEvent{entityA, entityB, CollisionState::Stay});
            }
        }
    }

    for (auto& key : activeCollisions) {
        if (!currentCollisions.contains(key)) {
            world.getEventManager().emit(CollisionEvent{key.first, key.second, CollisionState::Exit});
        }
    }

    activeCollisions = std::move(currentCollisions); // Update with current collisions.sssssss
}

std::vector<Entity*> CollisionSystem::queryCollidables(const std::vector<std::unique_ptr<Entity>> &entities) {
    std::vector<Entity*> collidables;
    for (auto& e : entities) {
        if (e->hasComponent<Transform>() && e->hasComponent<Collider>()) {
            collidables.push_back(e.get());
        }
    }

    return collidables;
}
