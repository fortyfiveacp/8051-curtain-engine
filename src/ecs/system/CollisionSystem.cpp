#include "CollisionSystem.h"

#include <iostream>

#include "Collision.h"
#include "World.h"

// Has two important functions but they are related
// 1. Positions the collider with the transform
// 2. Checking for collisions

// This can be improved (e.g. for bullet hell)
void CollisionSystem::update(World& world) {
    // Get a list of entities that have colliders and transforms.
    const std::vector<Entity*> collidables = queryCollidables(world.getEntities());

    // Update all collider positions first
    for (auto entity : collidables) {
        auto& t = entity->getComponent<Transform>();

        if (entity->hasComponent<RectCollider>()) {
            auto& c = entity->getComponent<RectCollider>();

            c.rect.x = t.position.x + c.offset.x;
            c.rect.y = t.position.y + c.offset.y;
        }

        if (entity->hasComponent<CircleCollider>()) {
            auto& c = entity->getComponent<CircleCollider>();

            c.centerPosition.x = t.position.x + c.offset.x;
            c.centerPosition.y = t.position.y + c.offset.y;
        }
    }

    std::set<CollisionKey> currentCollisions;

    // TODO: handle circle colliders

    // Outer loop:
    for (size_t i = 0; i < collidables.size(); i++) {
        // Update the collider position
        auto entityA = collidables[i];
        auto& t = entityA->getComponent<Transform>();
        auto& colliderA = entityA->getComponent<RectCollider>();

        //colliderA.rect.x = t.position.x + c.offset.x; // TODO: Isn't this redundant?
        //colliderA.rect.y = t.position.y + c.offset.y;

        // Check for collider collision.
        // Inner loop:
        for (size_t j = i + 1; j < collidables.size(); j++) {
            auto entityB = collidables[j];
            auto& colliderB = entityB->getComponent<RectCollider>();

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

    activeCollisions = std::move(currentCollisions); // Update with current collisions.
}

std::vector<Entity*> CollisionSystem::queryCollidables(const std::vector<std::unique_ptr<Entity>> &entities) {
    std::vector<Entity*> collidables;

    for (auto& e : entities) {
        if (e->hasComponent<Transform>() && (e->hasComponent<RectCollider>() || e->hasComponent<CircleCollider>())) {
            collidables.push_back(e.get());
        }
    }

    return collidables;
}
