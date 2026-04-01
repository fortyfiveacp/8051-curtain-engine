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
                    auto& v = entity->getComponent<Velocity>();

                    // If the entity is controlled by keyboard input, handle direction change first.
                    if (entity->hasComponent<KeyboardInput>()) {
                        auto& keyboardInput = entity->getComponent<KeyboardInput>();

                        // Determine horizontal direction, with left priority.
                        if (keyboardInput.left) {
                            v.direction.x = -1;
                        } else if (keyboardInput.right) {
                            v.direction.x = 1;
                        } else {
                            v.direction.x = 0;
                        }

                        // Determine vertical direction, with down priority.
                        if (keyboardInput.down) {
                            v.direction.y = 1;
                        } else if (keyboardInput.up) {
                            v.direction.y = -1;
                        } else {
                            v.direction.y = 0;
                        }

                        // Slow down current speed if in focus mode.
                        if (keyboardInput.focus) {
                            v.currentSpeed = v.baseSpeed * keyboardInput.focusMultiplier;
                        } else {
                            v.currentSpeed = v.baseSpeed;
                        }
                    }

                    // Update position.
                    updatePosition(dt, entity);
                }

                if (entity->hasComponent<AngularVelocity>()) {
                    // Update rotation based on angular velocity.
                    updateRotationBasedOnAngularVelocity(dt, entity);
                }
                else if (entity->hasComponent<LookAtRotator>()) {
                    // Update rotation to look at a specific target.
                    updateRotationBasedOnTarget(entity);
                }
            }
        }
    }

private:
    static void updatePosition(float dt, std::unique_ptr<Entity> &entity) {
        auto& t = entity->getComponent<Transform>();
        auto& v = entity->getComponent<Velocity>();

                // Track previous frame's position (for collisions).
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
        Vector2D velocityVector = directionVector * v.currentSpeed;

        // Update position.
        t.position += velocityVector * dt;
    }

    static void updateRotationBasedOnAngularVelocity(float dt, std::unique_ptr<Entity> &entity) {
        auto& t = entity->getComponent<Transform>();
        auto& v = entity->getComponent<AngularVelocity>();

        t.rotation = std::fmod(t.rotation + (v.rotationOverTime * dt), 360.0f);
    }

    static void updateRotationBasedOnTarget(std::unique_ptr<Entity> &entity) {
        auto& t = entity->getComponent<Transform>();
        auto& r = entity->getComponent<LookAtRotator>();

        const Vector2D displacement = r.target.position - t.position;

        t.rotation = std::fmod(std::atan2(displacement.y, displacement.x) * (180 / std::numbers::pi) +
            r.offsetDegrees - 90.0f, 360.0f);
    }
};
