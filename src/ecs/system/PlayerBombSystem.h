#pragma once
#include <vector>
#include <memory>
#include "Entity.h"
#include "Component.h"

class PlayerBombSystem {
public:
    void update(std::vector<std::unique_ptr<Entity>>& entities, float dt) {
        for (auto& entity : entities) {
            if (entity->hasComponent<PlayerBomb>()) {
                auto& bomb = entity->getComponent<PlayerBomb>();
                auto& fadeOut = entity->getComponent<Fade>();

                bomb.timer += dt;

                if (bomb.timer >= 2.0f && fadeOut.endingAlpha != 0) {
                    fadeOut.isFading = true;
                    fadeOut.startingAlpha = 100;
                    fadeOut.endingAlpha = 0;
                    fadeOut.fadeDuration = 1.0f;
                    fadeOut.durationTimer = 0;
                }

                if (bomb.timer >= bomb.duration) {
                    entity->destroy();
                }
            }
        }
    }
};