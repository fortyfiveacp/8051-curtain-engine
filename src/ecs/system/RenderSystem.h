#pragma once
#include <cmath>
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"
#include "../../manager/TextureManager.h"

class RenderSystem {
public:
    void render(std::vector<std::unique_ptr<Entity>>& entities) {
        Entity* cameraEntity = nullptr;

        // Find camera.
        for (auto& e : entities) {
            if (e->hasComponent<Camera>()) {
                cameraEntity = e.get();
                break;
            }
        }

        // No camera = no rendering.
        if (!cameraEntity) {
            return;
        }

        auto& cam = cameraEntity->getComponent<Camera>();

        for (auto& entity : entities) {
            if (entity->hasComponent<Transform>() && entity->hasComponent<Sprite>()) {
                auto& t = entity->getComponent<Transform>();
                auto& sprite = entity->getComponent<Sprite>();

                // Only render sprites on the World layer.
                if (sprite.renderLayer != RenderLayer::World) {
                    continue;
                }

                if (entity->hasComponent<InvincibilityFrames>()) {
                    auto& invincibilityFrames = entity->getComponent<InvincibilityFrames>();

                    // If the entity is currently invincible, make the sprite flicker by flipping between partial and full alpha.
                    if (invincibilityFrames.active) {
                        // Set the flicker frequency to 10 per second.
                        float flickerFrequency = 10.0f;

                        // Calculate flicker cycle duration.
                        // A flicker cycle starts at full alpha, then flips to partial alpha halfway through the cycle.
                        float cycleDuration = 1.0f / flickerFrequency;

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

                // We are converting from world space to screen space.
                sprite.dst.x = t.position.x - cam.view.x;
                sprite.dst.y = t.position.y - cam.view.y;

                // If the entity has animation, update the source rect.
                if (entity->hasComponent<Animation>()) {
                    auto& anim = entity->getComponent<Animation>();
                    sprite.src = anim.clips[anim.currentClip].frameIndices[anim.currentFrame];
                }

                TextureManager::draw(sprite.texture, &sprite.src, &sprite.dst);
            }
        }
    }
};
