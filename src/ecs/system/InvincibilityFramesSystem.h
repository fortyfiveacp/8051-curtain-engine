#pragma once
#include <memory>
#include <vector>

#include "Component.h"

class InvincibilityFramesSystem {
public:
    void update(std::vector<std::unique_ptr<Entity>>& entities, float dt) {
        for (auto& entity : entities) {
            if (entity->hasComponent<InvincibilityFrames>()) {
                auto& invincibilityFrames = entity->getComponent<InvincibilityFrames>();

                if (invincibilityFrames.active) {
                    invincibilityFrames.timer += dt;

                    if (invincibilityFrames.timer >= invincibilityFrames.duration) {
                        invincibilityFrames.active = false;
                        invincibilityFrames.timer = 0.0f;
                    }
                }
            }
        }
    }
};
