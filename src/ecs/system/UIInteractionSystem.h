#pragma once
#include <memory>

#include "Component.h"
#include "Entity.h"

class UIInteractionSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, const SDL_Event& event) {
        Selectable* currentlySelected = nullptr;

        // Bring up pause menu with escape.
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
            for (auto& e : entities) {
                if (e->hasComponent<Toggleable>()) {

                    e->getComponent<Toggleable>().toggle();
                    break;
                }
            }
        }

        for (auto& e : entities) {
            // Find currently selected entity.
            if (e->hasComponent<Selectable>()) {
                auto& s = e->getComponent<Selectable>();
                if (s.selected) {
                    currentlySelected = &s;
                    break;
                }
            }
        }

        // UI interaction with keyboard.
        if (currentlySelected && event.type == SDL_EVENT_KEY_DOWN) {
            switch (event.key.key) {
                case SDLK_RETURN:
                    currentlySelected->onPresssed();
                    break;
                case SDLK_UP:
                    currentlySelected->selected = false;
                    currentlySelected->onReleased();

                    currentlySelected->previous->onSelect();
                    currentlySelected->previous->selected = true;
                    break;
                case SDLK_DOWN:
                    currentlySelected->selected = false;
                    currentlySelected->onReleased();

                    currentlySelected->next->onSelect();
                    currentlySelected->next->selected = true;
                    break;
                default:
                    break;
            }
        }
    }
};