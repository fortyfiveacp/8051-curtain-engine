#pragma once
#include <string>
#include <unordered_map>

#include "SDL3_mixer/SDL_mixer.h"

class AudioManager {
    MIX_Mixer* mixer = nullptr;
    MIX_Track* musicTrack = nullptr;
    static MIX_Track* sfxTrack;
    static std::unordered_map<std::string, MIX_Audio*> audio;

public:
    AudioManager();

    void loadAudio(const std::string& name, const char* path) const;
    void playMusic(const std::string& name) const;
    void stopMusic(const std::string& name) const;

    static void playSfx(const std::string& name);
};
