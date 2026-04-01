#include "StageUtils.h"

#include "World.h"
#include "manager/TextureManager.h"
#include "SDL3/SDL_render.h"

Entity& StageUtils::createStageBackground(World& world, float stageWidth, float stageHeight, float startingY, float scrollSpeedY, const char* texturePath) {
    SDL_Texture* tex = TextureManager::load(texturePath);
    SDL_FRect src {0, 0, static_cast<float>(tex->w), static_cast<float>(tex->h)};
    SDL_FRect dst = {0, 0, stageWidth, stageHeight};

    // Create backgrounds.
    auto& stageBackground = world.createEntity();
    stageBackground.addComponent<Transform>(Vector2D(0, startingY), 0.0f, 1.0f);
    stageBackground.addComponent<Velocity>(Vector2D(0, 1), scrollSpeedY);
    stageBackground.addComponent<Sprite>(tex, src, dst, RenderLayer::World);
    stageBackground.addComponent<StageBackground>(scrollSpeedY, 0.0f, tex);

    return stageBackground;
}
