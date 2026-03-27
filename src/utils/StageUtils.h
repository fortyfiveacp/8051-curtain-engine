#pragma once

class StageUtils {
public:
    static float CalculateStageWidth(int windowWidth) {
        return windowWidth * 0.6;
    }

    static float CalculateStageHeight(int windowHeight) {
        return windowHeight * 0.93;
    }
};