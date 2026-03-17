#include "MouseInputSystem.h"

#include "World.h"

void MouseInputSystem::update(World& world, const SDL_Event& event) {
    if (event.type != SDL_EVENT_MOUSE_MOTION &&
        event.type != SDL_EVENT_MOUSE_BUTTON_DOWN &&
        event.type != SDL_EVENT_MOUSE_BUTTON_UP) {
        return;
    }

    float mx, my;
    SDL_GetMouseState(&mx, &my);

    for (auto& entity : world.getEntities()) {
        if (entity->hasComponent<Clickable>() && entity->hasComponent<Collider>()) {
            Clickable& clickable = entity->getComponent<Clickable>();
            Collider& collider = entity->getComponent<Collider>();

            if (!collider.enabled) {
                continue;
            }

            bool inside = (mx >= collider.rect.x && mx <= collider.rect.x + collider.rect.w &&
                my >= collider.rect.y && my <= collider.rect.y + collider.rect.h);

            // Hover.
            if (event.type == SDL_EVENT_MOUSE_MOTION) {
                if (!inside && clickable.pressed) {
                    world.getEventManager().emit(MouseInteractionEvent{entity.get(), MouseInteractionState::Cancel});
                }
            }

            // Pressed.
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (inside) {
                        clickable.pressed = true;
                        world.getEventManager().emit(MouseInteractionEvent{entity.get(), MouseInteractionState::Pressed});
                    }
                }
            }

            // Released.
            if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (inside) {
                        clickable.pressed = false;
                        world.getEventManager().emit(MouseInteractionEvent{entity.get(), MouseInteractionState::Released});
                    }
                }
            }
        }
    }
}
