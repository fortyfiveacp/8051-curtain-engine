#pragma once
#include <iostream>
#include <SDL3_mixer/SDL_mixer.h>

#include "manager/AssetManager.h"
#include "event/BaseEvent.h"

class World;

class AudioSystem {
public:
    AudioSystem(World& world);
    ~AudioSystem() {
        MIX_Quit();
    }
private:
    MIX_Mixer* mixer = nullptr;
    MIX_Track* musicTrack = nullptr;

    void handleAudioEvent(const BaseEvent& e) {
        if (const auto* sfx = dynamic_cast<const AudioEvent*>(&e)) {
            playSound(sfx->sound);
        }
        else if (const auto* musicEvent = dynamic_cast<const MusicEvent*>(&e)) {
            playMusic(*musicEvent);
        }
    }

    void playSound(SoundID id) {
        if (!mixer) return;
        MIX_Audio* audio = AssetManager::getSound(id);
        if (audio) MIX_PlayAudio(mixer, audio);
    }

    void playMusic(const MusicEvent& musicEvent) {
        if (!mixer) {
            return;
        }

        if (musicTrack) {
            MIX_StopTrack(musicTrack, 0);
            MIX_DestroyTrack(musicTrack);
            musicTrack = nullptr;
        }

        if (musicEvent.action == MusicAction::Stop) {
            return;
        }

        MIX_Audio* audio = AssetManager::getMusic(musicEvent.music);
        if (!audio) {
            return;
        }

        musicTrack = MIX_CreateTrack(mixer);
        SDL_PropertiesID props = SDL_CreateProperties();
        if (musicEvent.loop) {
            SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
        }

        MIX_SetTrackAudio(musicTrack, audio);
        MIX_PlayTrack(musicTrack, props);
        SDL_DestroyProperties(props);
    }
};