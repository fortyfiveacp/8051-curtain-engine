#include "MainMenuSystem.h"

#include "Game.h"
#include "SDL3/SDL_events.h"

void MainMenuSystem::update(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.key) {
            case SDLK_Z:
            case SDLK_RETURN:
            case SDLK_SPACE:
                AudioManager::playSfx("ok");
                Game::onSceneChangeRequest("level1");
                break;
            case SDLK_TAB:
                AudioManager::playSfx("ok");
                Game::onSceneChangeRequest("credits");
                break;
            default:
                break;
        }
    }
}
