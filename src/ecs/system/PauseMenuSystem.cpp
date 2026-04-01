#include "PauseMenuSystem.h"

#include "World.h"
#include "event/BaseEvent.h"

void PauseMenuSystem::update(const std::vector<std::unique_ptr<Entity>>& entities, World& world, const SDL_Event& event, bool isPaused) {
    // Bring up pause menu and pause the game with the escape key.
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
        for (auto& e : entities) {
            if (e->hasComponent<PauseMenuTag>() && e->hasComponent<Toggleable>()) {
                auto& toggleable = e->getComponent<Toggleable>();

                if (toggleable.enabled) {
                    // Toggle pause menu UI.
                    toggleable.toggle();

                    // Toggle game pause via event.
                    world.getEventManager().emit(PauseEvent{!isPaused});
                    break;
                }
            }
        }
    }
}
