#pragma once
#include <memory>

#include "Component.h"
#include "Entity.h"

class PauseMenuSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, const SDL_Event& event) {
        // Bring up pause menu with the escape key.
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
            for (auto& e : entities) {
                if (e->hasComponent<PauseMenuTag>() && e->hasComponent<Toggleable>()) {
                    e->getComponent<Toggleable>().toggle();
                    break;
                }
            }
        }
    }
};