#pragma once
#include <memory>
#include <vector>

#include "Entity.h"
#include "Component.h"

class FadeSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, float dt) {
        for (auto& entity : entities) {
            if (entity->hasComponent<Fade>()) {
                auto& fade = entity->getComponent<Fade>();

                if (fade.isFading) {
                    Uint8 currentAlpha = fade.startingAlpha;

                    // Wait for delay first before starting fade.
                    fade.delayTimer += dt;

                    if (fade.delayTimer >= fade.fadeDelayDuration) {
                        fade.durationTimer += dt;

                        if (fade.durationTimer >= fade.fadeDuration) {
                            // Ensure exact ending alpha.
                            currentAlpha = fade.endingAlpha;

                            // Reset timers and stop fade.
                            fade.durationTimer = 0;
                            fade.delayTimer = 0;
                            fade.isFading = false;
                        } else {
                            float t = fade.durationTimer / fade.fadeDuration;
                            currentAlpha = fade.startingAlpha + (fade.endingAlpha - fade.startingAlpha) * t;
                        }
                    }

                    // Update the alpha of entities. Sprite, icon counter and label are supported.
                    // If the delay timer isn't up yet this will set the alpha to the starting value.
                    if (entity->hasComponent<Sprite>()) {
                        auto& sprite = entity->getComponent<Sprite>();
                        SDL_SetTextureAlphaMod(sprite.texture, currentAlpha);
                    } else if (entity->hasComponent<Label>()) {
                        auto& label = entity->getComponent<Label>();
                        SDL_SetTextureAlphaMod(label.texture, currentAlpha);
                    } else if (entity->hasComponent<IconCounter>() && entity->hasComponent<Children>()) {
                        for (auto& child : entity->getComponent<Children>().children) {
                            if (child->hasComponent<Sprite>()) {
                                auto& sprite = child->getComponent<Sprite>();
                                SDL_SetTextureAlphaMod(sprite.texture, currentAlpha);
                            }
                        }
                    } else {
                        std::cerr << "Entity trying to fade doesn't have a supported texture component!" << std::endl;
                    }
                }
            }
        }
    }
};
