#pragma once
#include <cmath>
#include <memory>
#include <vector>

#include "Component.h"

class InvincibilityFramesSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, const float dt) {
        for (auto& entity : entities) {
            if (entity->hasComponent<InvincibilityFrames>() && entity->hasComponent<Sprite>()) {
                auto& invincibilityFrames = entity->getComponent<InvincibilityFrames>();
                auto& sprite = entity->getComponent<Sprite>();

                if (invincibilityFrames.active) {
                    invincibilityFrames.timer += dt;

                    // When the timer is greater than or equal to the duration, turn off the invincibility and reset timer.
                    if (invincibilityFrames.timer >= invincibilityFrames.duration) {
                        invincibilityFrames.active = false;
                        invincibilityFrames.timer = 0.0f;
                    }

                    // Calculate flicker cycle duration.
                    // A flicker cycle starts at full alpha, then flips to partial alpha halfway through the cycle.
                    float cycleDuration = 1.0f / invincibilityFrames.flickerFrequency;

                    // Calculate if it's the first or second half of the cycle.
                    float positionInCycle = std::fmod(invincibilityFrames.timer, cycleDuration);
                    bool isFullAlpha = positionInCycle < (cycleDuration / 2.0f);

                    // Set alpha accordingly.
                    SDL_SetTextureAlphaMod(sprite.texture, isFullAlpha ? 255 : 100);
                } else {
                    // Ensure alpha is set back to full opacity when not invincible.
                    SDL_SetTextureAlphaMod(sprite.texture, 255);
                }
            }
        }
    }
};
