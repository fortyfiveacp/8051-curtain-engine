#pragma once
#include <memory>
#include <vector>

#include "Component.h"

class InvincibilityFramesSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, const float dt) {
        for (auto& entity : entities) {
            if (entity->hasComponent<InvincibilityFrames>()) {
                auto& invincibilityFrames = entity->getComponent<InvincibilityFrames>();

                if (invincibilityFrames.active) {
                    invincibilityFrames.timer += dt;

                    // When the timer is greater than or equal to the duration, turn off the invincibility and reset timer.
                    if (invincibilityFrames.timer >= invincibilityFrames.duration) {
                        invincibilityFrames.active = false;
                        invincibilityFrames.timer = 0.0f;
                    }
                }
            }
        }
    }
};
