#pragma once
#include <vector>

#include "SDL3/SDL_rect.h"

struct AnimationClip {
    std::vector<SDL_FRect> frameIndices;
};
