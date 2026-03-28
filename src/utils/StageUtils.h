#pragma once

class StageUtils {
public:
    static float CalculateStageWidth(int windowWidth) {
        return windowWidth * 0.6;
    }

    static float CalculateStageHeight(int windowHeight) {
        return windowHeight * 0.93;
    }

    static SDL_Rect CalculateStageRect(int windowWidth, int windowHeight) {
        int stageWidth = CalculateStageWidth(windowWidth);
        int stageHeight = CalculateStageHeight(windowHeight);
        int paddingX = windowWidth * 0.05;
        int paddingY = (windowHeight - stageHeight) / 2;

        return { paddingX, paddingY, stageWidth, stageHeight };
    }
};