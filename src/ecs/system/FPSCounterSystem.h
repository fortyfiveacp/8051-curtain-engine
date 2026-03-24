#pragma once
#include <iomanip>
#include <memory>
#include <sstream>
#include <vector>

#include "Component.h"
#include "Entity.h"

class FPSCounterSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, float dt) {
        for (auto& e : entities) {
            if (e->hasComponent<FPSCounter>() && e->hasComponent<Label>()) {
                auto& fpsCounter = e->getComponent<FPSCounter>();
                auto& label = e->getComponent<Label>();

                fpsCounter.frameCount++;
                fpsCounter.timer += dt;

                // Only update FPS counter every second.
                if (fpsCounter.timer >= 1.0f) {
                    float averageFPS = static_cast<float>(fpsCounter.frameCount) / fpsCounter.timer;

                    // Update FPS counter label.
                    if (label.type == LabelType::FPSCounter) {
                        std::stringstream ss;
                        ss << std::fixed << std::setprecision(3) << averageFPS << "fps";
                        label.text = ss.str();
                        label.dirty = true;

                        // Reset frame count and timer.
                        fpsCounter.frameCount = 0;
                        fpsCounter.timer = 0.0f;
                    }
                }
            }
        }
    }
};