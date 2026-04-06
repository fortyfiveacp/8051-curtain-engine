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

        auto& playerStats = playerEntity->getComponent<PlayerStats>();

        // Update the HUD labels text.
        for (auto& e : entities) {
            if (e->hasComponent<Label>()) {
                auto& label = e->getComponent<Label>();
                std::stringstream ss;

                // Only perform the update if the new value is actually different.
                if (label.type == LabelType::HiScore && playerStats.currentHiScore != std::stoi(label.text)) {
                    ss << std::setw(9) << std::setfill('0') << playerStats.currentHiScore;

                    label.text = ss.str();
                    label.dirty = true;
                } else if (label.type == LabelType::Score && playerStats.currentScore != std::stoi(label.text)) {
                    ss << std::setw(9) << std::setfill('0') << playerStats.currentScore;

                    label.text = ss.str();
                    label.dirty = true;
                } else if (label.type == LabelType::Power && playerStats.currentPower != std::stoi(label.text)) {
                    ss << playerStats.currentPower;

                    label.text = ss.str();
                    label.dirty = true;
                } else if (label.type == LabelType::Graze && playerStats.currentGraze != std::stoi(label.text)) {
                    ss << playerStats.currentGraze;

                    label.text = ss.str();
                    label.dirty = true;
                } else if (label.type == LabelType::Point && playerStats.currentPoint != std::stoi(label.text)) {
                    ss << playerStats.currentPoint;

                    label.text = ss.str();
                    label.dirty = true;
                }
            }
        }
    }
};
