#pragma once
#include <memory>

#include "Component.h"
#include "Entity.h"

class MovementInputSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, const SDL_Event& event) {
        for (auto& e : entities) {
            // Change direction of the player.
            if (e->hasComponent<PlayerTag>() && e->hasComponent<MovementInput>() && e->hasComponent<Velocity>()) {
                auto& moveInput = e->getComponent<MovementInput>();
                auto& v = e->getComponent<Velocity>();

                // Handle key down events, taking advantage of fallthrough so both WASD and arrow keys work.
                if (event.type == SDL_EVENT_KEY_DOWN) {
                    switch (event.key.key) {
                        case SDLK_UP:
                        case SDLK_W:
                            moveInput.up = true;
                            break;
                        case SDLK_DOWN:
                        case SDLK_S:
                            moveInput.down = true;
                            break;
                        case SDLK_LEFT:
                        case SDLK_A:
                            moveInput.left = true;
                            break;
                        case SDLK_RIGHT:
                        case SDLK_D:
                            moveInput.right = true;
                            break;
                        case SDLK_LSHIFT:
                        case SDLK_RSHIFT:
                            moveInput.focus = true;
                            break;
                        default:
                            break;
                    }
                }

                if (event.type == SDL_EVENT_KEY_UP) {
                    switch (event.key.key) {
                        case SDLK_UP:
                        case SDLK_W:
                            moveInput.up = false;
                            break;
                        case SDLK_DOWN:
                        case SDLK_S:
                            moveInput.down = false;
                            break;
                        case SDLK_LEFT:
                        case SDLK_A:
                            moveInput.left = false;
                            break;
                        case SDLK_RIGHT:
                        case SDLK_D:
                            moveInput.right = false;
                            break;
                        case SDLK_LSHIFT:
                        case SDLK_RSHIFT:
                            moveInput.focus = false;
                            break;
                        default:
                            break;
                    }
                }

                // Determine horizontal direction, with left priority.
                if (moveInput.left) {
                    v.direction.x = -1;
                } else if (moveInput.right) {
                    v.direction.x = 1;
                } else {
                    v.direction.x = 0;
                }

                // Determine vertical direction, with down priority.
                if (moveInput.down) {
                    v.direction.y = 1;
                } else if (moveInput.up) {
                    v.direction.y = -1;
                } else {
                    v.direction.y = 0;
                }

                // Slow down current speed if in focus mode.
                if (moveInput.focus) {
                    v.currentSpeed = v.baseSpeed * moveInput.focusMultiplier;
                } else {
                    v.currentSpeed = v.baseSpeed;
                }
            }
        }
    }
};