#pragma once
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class ItemBounceSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, float dt) {
        for (auto& entity : entities) {
            if (entity->hasComponent<Item>() && entity->hasComponent<Velocity>()) {
                auto& item = entity->getComponent<Item>();
                auto& velocity = entity->getComponent<Velocity>();

                if (item.isBouncing) {
                    // While bouncing, counting down the timer decreases speed as it bounces.
                    item.timer -= dt;

                    if (item.timer <= 0) {
                        // After the bounce duration is up, change direction to drop downwards.
                        velocity.direction = Vector2D(0, 1);
                        item.isBouncing = false;
                    }
                } else {
                    // After bouncing, counting up the timer increases speed as it falls.
                    item.timer += dt;
                }

                // Modify the item's current speed based on the timer.
                // While the item is bouncing up, reduce its speed as it gets closer to the end of the bounce.
                // After bouncing, ramp up its speed as it falls.
                velocity.currentSpeed = velocity.baseSpeed * (item.timer / item.bounceDuration);
            }
        }
    }
};
