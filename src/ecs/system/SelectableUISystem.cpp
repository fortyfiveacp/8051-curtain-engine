#include "SelectableUISystem.h"

#include "World.h"
#include "event/BaseEvent.h"

void SelectableUISystem::update(const std::vector<std::unique_ptr<Entity> > &entities, World& world, const SDL_Event &event) {
    Entity* currentlySelectedEntity = nullptr;
    SelectableUI *currentlySelectedUI = nullptr;

    // Find currently selected UI.
    for (auto &e: entities) {
        if (e->hasComponent<SelectableUI>()) {
            currentlySelectedEntity = e.get();
            auto &selectableUI = e->getComponent<SelectableUI>();

            if (selectableUI.selected) {
                currentlySelectedUI = &selectableUI;
                break;
            }
        }
    }

    // UI interaction with keyboard via events.
    if (currentlySelectedUI && event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.key) {
            case SDLK_RETURN:
                world.getEventManager().emit(UIInteractionEvent{currentlySelectedEntity, UIInteractionState::Pressed});
                break;
            case SDLK_UP:
                world.getEventManager().emit(UIInteractionEvent{currentlySelectedEntity, UIInteractionState::Released});
                world.getEventManager().emit(UIInteractionEvent{currentlySelectedUI->previous, UIInteractionState::Selected});
                break;
            case SDLK_DOWN:
                world.getEventManager().emit(UIInteractionEvent{currentlySelectedEntity, UIInteractionState::Released});
                world.getEventManager().emit(UIInteractionEvent{currentlySelectedUI->next, UIInteractionState::Selected});
                break;
            default:
                break;
        }
    }
}
