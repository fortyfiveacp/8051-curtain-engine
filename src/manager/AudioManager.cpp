#include "AudioManager.h"

#include <iostream>
#include <ostream>
#include <unordered_map>
#include <chrono>

MIX_Mixer* AudioManager::mixer = nullptr;
MIX_Track* AudioManager::musicTrack = nullptr;
std::vector<MIX_Track*> AudioManager::sfxTracks;
std::unordered_map<std::string, MIX_Audio*> AudioManager::audio;
std::unordered_map<std::string, double> AudioManager::sfxLastPlayedTimes;
constexpr int MAX_SFX_TRACKS = 32;
constexpr float BGM_VOLUME = 0.7f;
constexpr float SFX_BASE_VOLUME = 0.3f;

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

    MIX_SetTrackGain(musicTrack, BGM_VOLUME);
}

void AudioManager::loadAudio(const std::string &name, const char *path) {
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

void AudioManager::playMusic(const std::string &name) {
    if (MIX_SetTrackAudio(musicTrack, audio[name]) == 0) {
        std::cout << "MIX_SetTrackAudio() Failed" << std::endl;
        return;
    }
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1); // -1 means loop endless.

    MIX_PlayTrack(musicTrack, props);
    std::cout << "Playing music: " << name << std::endl;

    SDL_DestroyProperties(props);
}

void AudioManager::pauseMusic() {
    if (musicTrack) {
        MIX_PauseTrack(musicTrack);
        std::cout << "Music paused." << std::endl;
    }
}

void AudioManager::resumeMusic() {
    if (musicTrack) {
        MIX_ResumeTrack(musicTrack);
        std::cout << "Music resumed." << std::endl;
    }
}

void AudioManager::stopMusic() {
    MIX_StopTrack(musicTrack, 0);
}

void AudioManager::playSfx(const std::string &name, float volume) {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    double currentTimeMS = std::chrono::duration<double, std::milli>(now).count();
    double cooldownMS = 25.0;

    /*
     * Check if a specific sound effect is being played at the same frame.
     * Skip playing the sound effect if it is, preventing sfx stacking and
     * bleeding ears.
     */
    if (sfxLastPlayedTimes.contains(name)) {
        if (currentTimeMS - sfxLastPlayedTimes[name] < cooldownMS) {
            return;
        }
    }

    for (auto* track : sfxTracks) {
        if (!MIX_TrackPlaying(track)) {
            if (MIX_SetTrackAudio(track, audio[name]) == 0) {
                return;
            }
            MIX_SetTrackGain(track, SFX_BASE_VOLUME * volume);
            MIX_PlayTrack(track, 0);
            sfxLastPlayedTimes[name] = currentTimeMS;
            return;
        }
    }
}