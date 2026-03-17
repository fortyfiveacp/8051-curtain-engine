#pragma once
#include "SDL3/SDL_events.h"

class World;

class MouseInputSystem {
public:
    void update(World& world, const SDL_Event& event);
};
