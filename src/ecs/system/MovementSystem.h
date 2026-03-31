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

                // If the entity is controlled by keyboard input, handle direction change.
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

                // Track previous frame's position (for collisions).
                t.oldPosition = t.position;

                Vector2D directionVector = v.direction;

                // Normalize.
                directionVector.normalize();

                // Vector2D needs an operator function to multiply a float.
                Vector2D velocityVector = directionVector * v.currentSpeed;

                // Update position.
                t.position += velocityVector * dt;
            }
        }
    }
};
