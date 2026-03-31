#pragma once
#include <memory>

#include "Component.h"
#include "Entity.h"

class KeyboardInputSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, const SDL_Event& event) {
        for (auto& e : entities) {
            // Get the keyboard input.
            if (e->hasComponent<PlayerTag>() && e->hasComponent<KeyboardInput>()) {
                auto& keyboardInput = e->getComponent<KeyboardInput>();

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
            }
        }
    }
};