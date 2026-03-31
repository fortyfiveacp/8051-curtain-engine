#pragma once
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class MovementSystem {
public:
    void update(std::vector<std::unique_ptr<Entity>>& entities, float dt) {
        for (auto& entity : entities) {
            if (entity->hasComponent<Transform>()) {
                if (entity->hasComponent<Velocity>()) {
                    // Update position.
                    updatePosition(dt, entity);
                }
                if (entity->hasComponent<AngularVelocity>()) {
                    // Update rotation.
                    updateRotation(dt, entity);
                }
            }
        }
    }

private:
    static void updatePosition(float dt, std::unique_ptr<Entity> &entity) {
        auto& t = entity->getComponent<Transform>();
        auto& v = entity->getComponent<Velocity>();

        // Track previous frame's position (for collisions)
        t.oldPosition = t.position;

        Vector2D directionVector = v.direction;

        // Normalize.
        directionVector.normalize();

        // If local space is on, add rotation to the direction.
        if (v.isLocalSpace) {
            float rotationRadians = std::atan2(directionVector.y, directionVector.x);
            rotationRadians += t.rotation * (std::numbers::pi / 180.0f);
            std::fmod(rotationRadians, 360.0f);

            directionVector.x = std::cos(rotationRadians);
            directionVector.y = std::sin(rotationRadians);
        }

        // Vector2D needs an operator function to multiply a float.
        Vector2D velocityVector = directionVector * v.speed;

        // Update position.
        t.position += velocityVector * dt;
    }

    static void updateRotation(float dt, std::unique_ptr<Entity> &entity) {
        auto& t = entity->getComponent<Transform>();
        auto& v = entity->getComponent<AngularVelocity>();

        t.rotation = std::fmod(t.rotation + (v.rotationOverTime * dt), 360.0f);
    }
};
