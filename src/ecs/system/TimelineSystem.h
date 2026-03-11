#pragma once
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class TimelineSystem {
public:
    void update(std::vector<std::unique_ptr<Entity>>& entities, float dt) {
        for (auto& entity : entities) {
            if (entity->hasComponent<Timeline>()) {
                auto &[currentTime, timeline] = entity->getComponent<Timeline>();

                const float timeBeforeThisFrame = currentTime;
                currentTime += dt;

                const float differenceBetweenFrames = currentTime - timeBeforeThisFrame;

                for (const auto &[timestamp, action]: timeline) {
                    // If this action should occur between last frame and this frame.
                    if (currentTime - timestamp < differenceBetweenFrames && currentTime - timestamp >= 0) {
                        action();
                    }
                }
            }
        }
    }
};
