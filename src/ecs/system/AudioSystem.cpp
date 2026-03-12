#include "AudioSystem.h"

#include <iostream>

#include "World.h"

AudioSystem::AudioSystem(World &world) {
    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    world.getEventManager().subscribe(
        [this](const BaseEvent& e)
        {
            if (e.type != EventType::Audio)
                return;

            handleAudioEvent(e);
        }
    );
}
