#pragma once

#include <functional>

#include "Component.h"
#include "Entity.h"
#include "SDL3/SDL_pixels.h"

class World;

class UIUtils {
public:
    static void updateIconCounter(Entity& entity);

    // Functions for creating UI elements.
    static Entity& createSelectableButton(World& world, Entity& overlay, const char* font, SDL_Color selectedColour,
        SDL_Color unselectedColour, const char* text, const char* cacheKey, const std::function<void()>& onPressed);
    static Entity& createLabel(World& world, int x, int y, SDL_Color colour, const char* fontName, const char* text,
        const char* cacheKey, LabelType labelType);
    static Entity& createIconLabel(World& world, int x, int y, int maxNumber, float iconWidth, float iconHeight,
        IconCounterType iconCounterType, const char* texturePath);
};
