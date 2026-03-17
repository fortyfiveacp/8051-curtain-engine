#pragma once
#include <memory>

#include "Component.h"
#include "Entity.h"

class PauseMenuSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, const SDL_Event& event) {
        SelectableUI* currentlySelected = nullptr;

        // Bring up pause menu with escape.
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
            for (auto& e : entities) {
                if (e->hasComponent<PauseMenuTag>() && e->hasComponent<Toggleable>()) {
                    e->getComponent<Toggleable>().toggle();
                    break;
                }
            }
        }

        // Find currently selected UI.
        for (auto& e : entities) {
            if (e->hasComponent<SelectableUI>()) {
                auto& selectableUI = e->getComponent<SelectableUI>();
                if (selectableUI.selected) {
                    currentlySelected = &selectableUI;
                    break;
                }
            }
        }

        // UI interaction with keyboard.
        if (currentlySelected && event.type == SDL_EVENT_KEY_DOWN) {
            switch (event.key.key) {
                case SDLK_RETURN:
                    currentlySelected->onPressed();
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