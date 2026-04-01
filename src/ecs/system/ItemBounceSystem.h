#pragma once
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class ItemBounceSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, float dt) {
        for (auto& entity : entities) {
            if (entity->hasComponent<ItemBounce>() && entity->hasComponent<Velocity>()) {
                auto& itemBounce = entity->getComponent<ItemBounce>();
                auto& velocity = entity->getComponent<Velocity>();

                if (itemBounce.isBouncing) {
                    // While bouncing, counting down the timer decreases speed as it bounces.
                    itemBounce.timer -= dt;

                    if (itemBounce.timer <= 0) {
                        // After the bounce duration is up, change direction to drop downwards.
                        velocity.direction = Vector2D(0, 1);
                        itemBounce.isBouncing = false;
                    }
                } else {
                    // After bouncing, counting up the timer increases speed as it falls.
                    itemBounce.timer += dt;
                }

                // Modify the item's current speed based on the timer.
                // While the item is bouncing up, reduce its speed as it gets closer to the end of the bounce.
                // After bouncing, ramp up its speed as it falls.
                velocity.currentSpeed = velocity.baseSpeed * (itemBounce.timer / itemBounce.bounceDuration);
            }
        }
    }
};
