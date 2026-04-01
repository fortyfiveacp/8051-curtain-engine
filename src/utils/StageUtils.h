#pragma once
#include "Entity.h"
#include "SDL3/SDL_rect.h"

class World;

class StageUtils {
public:
    static float CalculateStageWidth(int windowWidth) {
        return windowWidth * 0.6;
    }

    static float CalculateStageHeight(int windowHeight) {
        return windowHeight * 0.93;
    }

    static float CalculateStagePaddingX(int windowWidth) {
        return windowWidth * 0.05;
    }

    static float CalculateStagePaddingY(int windowHeight) {
        return (windowHeight - CalculateStageHeight(windowHeight)) / 2;
    }

    static SDL_FRect CalculateStageRect(int windowWidth, int windowHeight) {
        float stageWidth = CalculateStageWidth(windowWidth);
        float stageHeight = CalculateStageHeight(windowHeight);
        float paddingX = windowWidth * 0.05;
        float paddingY = ((windowHeight - stageHeight) / 2) - 1;

        return {paddingX, paddingY, stageWidth, stageHeight};
    }

    static Entity& createStageBackground(World& world, float stageWidth, float stageHeight, float startingY,
        float scrollSpeedY, const char* texturePath);
};
