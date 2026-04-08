#include "CreditsSystem.h"

#include "Game.h"
#include "SDL3/SDL_events.h"

void CreditsSystem::update(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.key) {
            case SDLK_RETURN:
            case SDLK_ESCAPE:
                AudioManager::playSfx("ok");
                Game::onSceneChangeRequest("mainmenu");
                break;
            default:
                break;
        }
    }
}