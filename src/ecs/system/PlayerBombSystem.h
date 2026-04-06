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

                bomb.timer += dt;
                if (bomb.timer >= bomb.duration) {
                    entity->destroy();
                }
            }
        }
    }
};