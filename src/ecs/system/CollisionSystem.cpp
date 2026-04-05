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
    const auto& [rectCollidables, circleCollidables] = queryCollidables(world.getEntities());

    // Update all collider positions first
    for (auto entity : rectCollidables) {
        auto& t = entity->getComponent<Transform>();

        if (entity->hasComponent<RectCollider>()) {
            auto& c = entity->getComponent<RectCollider>();

            c.rect.x = t.position.x + c.offset.x;
            c.rect.y = t.position.y + c.offset.y;
        }
    }

    for (auto entity : circleCollidables) {
        auto& t = entity->getComponent<Transform>();

        if (entity->hasComponent<CircleCollider>()) {
            auto& c = entity->getComponent<CircleCollider>();

            c.centerPosition.x = t.position.x + c.offset.x;
            c.centerPosition.y = t.position.y + c.offset.y;
        }
    }

    std::set<CollisionKey> currentCollisions;

    // Outer loop:
    for (size_t i = 0; i < rectCollidables.size(); i++) {
        auto entityA = rectCollidables[i];
        auto& colliderA = entityA->getComponent<RectCollider>();

        // Check for rect x rect collision.
        // Inner loop:
        for (size_t j = i + 1; j < rectCollidables.size(); j++) {
            auto entityB = rectCollidables[j];
            auto& colliderB = entityB->getComponent<RectCollider>();

            if (Collision::AABB(colliderA, colliderB)) {
                emitOnCollisionEvent(*entityA, *entityB, currentCollisions, world);
            }
        }

        // Check for rect x circle collision.
        for (auto entityB : circleCollidables) {
            auto& colliderB = entityB->getComponent<CircleCollider>();

            if (Collision::AABBCircle(colliderA, colliderB)) {
                emitOnCollisionEvent(*entityA, *entityB, currentCollisions, world);
            }
        }
    }

    // Check for circle x circle collision.
    for (size_t i = 0; i < circleCollidables.size(); i++) {
        auto entityA = circleCollidables[i];
        auto& colliderA = entityA->getComponent<CircleCollider>();

        for (size_t j = i + 1; j < circleCollidables.size(); j++) {
            auto entityB = circleCollidables[j];
            auto& colliderB = entityB->getComponent<CircleCollider>();

            if (Collision::Circle(colliderA, colliderB)) {
                emitOnCollisionEvent(*entityA, *entityB, currentCollisions, world);
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

void CollisionSystem::emitOnCollisionEvent(Entity& entityA, Entity& entityB, std::set<CollisionKey>& currentCollisions, World& world) {
    CollisionKey key = makeKey(&entityA, &entityB);
    currentCollisions.insert(key);

    if (!activeCollisions.contains(key)) {
        world.getEventManager().emit(CollisionEvent{&entityA, &entityB, CollisionState::Enter});
    }

    world.getEventManager().emit(CollisionEvent{&entityA, &entityB, CollisionState::Stay});
}

std::pair<std::vector<Entity*>, std::vector<Entity*>> CollisionSystem::queryCollidables(const std::vector<std::unique_ptr<Entity>> &entities) {
    std::vector<Entity*> rectCollidables;
    std::vector<Entity*> circleCollidables;

    for (auto& e : entities) {
        if (e->hasComponent<Transform>()) {
            if (e->hasComponent<RectCollider>()) {
                rectCollidables.push_back(e.get());
            }
            if (e->hasComponent<CircleCollider>()) {
                circleCollidables.push_back(e.get());
            }
        }
    }

    return {rectCollidables, circleCollidables};
}
