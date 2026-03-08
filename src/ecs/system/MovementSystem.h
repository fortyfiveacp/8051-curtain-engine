#pragma once
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class MovementSystem {
public:
    void update(std::vector<std::unique_ptr<Entity>>& entities, float dt) {
        for (auto& entity : entities) {
            if (entity->hasComponent<Transform>() && entity->hasComponent<Velocity>()) {
                auto& t = entity->getComponent<Transform>();
                auto& v = entity->getComponent<Velocity>();
                // t.position.x += 60 * dt;
                // t.position.y += 60 * dt;

                // Track previous frame's position (for collisions)
                t.oldPosition = t.position;

                Vector2D directionVector = v.direction;

                // Normalize
                directionVector.normalize();

                // Vector2D needs an operator function to multiply a float.
                Vector2D velocityVector = directionVector * v.speed;

                // Update position
                t.position += velocityVector * dt;
            }
        }
    }
};
