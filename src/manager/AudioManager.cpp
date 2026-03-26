#include "AudioManager.h"

#include <iostream>
#include <ostream>

MIX_Track* AudioManager::sfxTrack;
std::unordered_map<std::string, MIX_Audio*> AudioManager::audio;

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
    sfxTrack = MIX_CreateTrack(mixer);
    MIX_SetTrackGain(musicTrack, 1.0f);
}

void AudioManager::loadAudio(const std::string &name, const char *path) const {
    if (audio.contains(path)) {
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
    std::cout << "Playing music: " << name << std::endl;
}

void AudioManager::stopMusic(const std::string &name) const {
    MIX_StopTrack(musicTrack, 0);
}

void AudioManager::playSfx(const std::string &name) {
    if (MIX_SetTrackAudio(sfxTrack, audio[name]) == 0) {
        std::cout << "MIX_SetTrackAudio() Failed" << std::endl;
        return;
    }

    MIX_PlayTrack(sfxTrack, 0);
    std::cout << "Playing sfx: " << name << std::endl;
}
