#pragma once
#include <cmath>
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class StageBackgroundSystem {
public:
    void update(std::vector<std::unique_ptr<Entity>>& entities, float dt) {
        for (auto& entity : entities) {
            if (entity->hasComponent<StageBackground>() && entity->hasComponent<Transform>() && entity->hasComponent<Sprite>()) {
                auto& stageBackground = entity->getComponent<StageBackground>();
                auto& transform = entity->getComponent<Transform>();
                auto& sprite = entity->getComponent<Sprite>();

                // Update background position.
                float offset = stageBackground.scrollSpeedY * dt;
                transform.position.y += offset;

                // Track how far the background has moved down.
                stageBackground.offsetY += offset;

                // Move background back to the top to loop it.
                if (stageBackground.offsetY > sprite.dst.h) {
                    transform.position.y -= sprite.dst.h;
                    stageBackground.offsetY -= sprite.dst.h;
                }
            }
        }
    }
};
