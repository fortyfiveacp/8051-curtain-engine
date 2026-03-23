#pragma once
#include <memory>
#include <sstream>
#include <vector>

#include "Component.h"
#include "Entity.h"

class HUDSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities) {
        Entity* playerEntity = nullptr;

        // Find the player.
        for (auto& e : entities) {
            if (e->hasComponent<PlayerTag>()) {
                playerEntity = e.get();
                break;
            }
        }

        if (!playerEntity) {
            return;
        }

        auto& playerTransform = playerEntity->getComponent<Transform>();

        for (auto& e : entities) {
            if (e->hasComponent<Label>()) {
                auto& label = e->getComponent<Label>();

                // Update player position label.
                if (label.type == LabelType::PlayerPosition) {
                    std::stringstream ss;
                    ss << "Player position: " << playerTransform.position;
                    label.text = ss.str();
                    label.dirty = true;
                }
            }
        }
    }
};
