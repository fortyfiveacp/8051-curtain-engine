#pragma once
#include <memory>

#include "Component.h"
#include "Entity.h"

class KeyboardInputSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, const SDL_Event& event) {
        for (auto& e : entities) {
            // Change direction of the player.
            if (e->hasComponent<PlayerTag>() && e->hasComponent<KeyboardInput>() && e->hasComponent<Velocity>()) {
                auto& keyboardInput = e->getComponent<KeyboardInput>();
                auto& v = e->getComponent<Velocity>();

                if (event.type == SDL_EVENT_KEY_DOWN) {
                    switch (event.key.key) {
                        case SDLK_UP:
                        case SDLK_W:
                            keyboardInput.up = true;
                            break;
                        case SDLK_DOWN:
                        case SDLK_S:
                            keyboardInput.down = true;
                            break;
                        case SDLK_LEFT:
                        case SDLK_A:
                            keyboardInput.left = true;
                            break;
                        case SDLK_RIGHT:
                        case SDLK_D:
                            keyboardInput.right = true;
                            break;
                        case SDLK_LSHIFT:
                        case SDLK_RSHIFT:
                            keyboardInput.focus = true;
                            break;
                        case SDLK_Z:
                            keyboardInput.shoot = true;
                            break;
                        case SDLK_X:
                            keyboardInput.bomb = true;
                            break;
                        default:
                            break;
                    }
                }

                if (event.type == SDL_EVENT_KEY_UP) {
                    switch (event.key.key) {
                        case SDLK_UP:
                        case SDLK_W:
                            keyboardInput.up = false;
                            break;
                        case SDLK_DOWN:
                        case SDLK_S:
                            keyboardInput.down = false;
                            break;
                        case SDLK_LEFT:
                        case SDLK_A:
                            keyboardInput.left = false;
                            break;
                        case SDLK_RIGHT:
                        case SDLK_D:
                            keyboardInput.right = false;
                            break;
                        case SDLK_LSHIFT:
                        case SDLK_RSHIFT:
                            keyboardInput.focus = false;
                            break;
                        case SDLK_Z:
                            keyboardInput.shoot = false;
                            break;
                        default:
                            break;
                    }
                }

                // Determine horizontal direction, with left priority.
                if (keyboardInput.left) {
                    v.direction.x = -1;
                } else if (keyboardInput.right) {
                    v.direction.x = 1;
                } else {
                    v.direction.x = 0;
                }

                // Determine vertical direction, with down priority.
                if (keyboardInput.down) {
                    v.direction.y = 1;
                } else if (keyboardInput.up) {
                    v.direction.y = -1;
                } else {
                    v.direction.y = 0;
                }

                // Slow down current speed if in focus mode.
                if (keyboardInput.focus) {
                    v.currentSpeed = v.baseSpeed * keyboardInput.focusMultiplier;
                } else {
                    v.currentSpeed = v.baseSpeed;
                }
            }
        }
    }
};