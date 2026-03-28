#include "AudioManager.h"

#include <iostream>
#include <ostream>

std::vector<MIX_Track*> AudioManager::sfxTracks;
std::unordered_map<std::string, MIX_Audio*> AudioManager::audio;
constexpr int MAX_SFX_TRACKS = 32;

AudioManager::AudioManager() {
    if (MIX_Init() == 0) {
        std::cout << "MIX_Init() Failed" << std::endl;
        return;
    }

    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!mixer) {
        std::cout << "MIX_CreateMixerDevice() Failed" << std::endl;
        return;
    }

    musicTrack = MIX_CreateTrack(mixer);

    for (int i = 0; i < MAX_SFX_TRACKS; i++) {
        sfxTracks.push_back(MIX_CreateTrack(mixer));
    }

    MIX_SetTrackGain(musicTrack, 1.0f);
}

void AudioManager::loadAudio(const std::string &name, const char *path) const {
    if (audio.contains(name)) {
        return;
    }

    auto audioPtr = MIX_LoadAudio(mixer, path, true);
    if (!audioPtr) {
        std::cout << "MIX_LoadAudio() Failed" << std::endl;
        return;
    }
    audio.emplace(name, audioPtr);
}

void AudioManager::playMusic(const std::string &name) const {
    if (MIX_SetTrackAudio(musicTrack, audio[name]) == 0) {
        std::cout << "MIX_SetTrackAudio() Failed" << std::endl;
        return;
    }

    MIX_PlayTrack(musicTrack, -1); // -1 means loop endless.
    MIX_TrackLooping(musicTrack);
    std::cout << "Playing music: " << name << std::endl;
}

void AudioManager::stopMusic(const std::string &name) const {
    MIX_StopTrack(musicTrack, 0);
}

void AudioManager::playSfx(const std::string &name) {
    for (auto* track : sfxTracks) {
        if (!MIX_TrackPlaying(track)) {
            if (MIX_SetTrackAudio(track, audio[name]) == 0) {
                std::cout << "MIX_SetTrackAudio() Failed" << std::endl;
                return;
            }

            MIX_PlayTrack(track, 0);
            std::cout << "Playing sfx: " << name << std::endl;
            return;
        }
    }
}
