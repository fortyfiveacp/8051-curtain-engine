#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "SDL3_mixer/SDL_mixer.h"

class AudioManager {
    static MIX_Mixer* mixer;
    static MIX_Track* musicTrack;

    static std::vector<MIX_Track*> sfxTracks;
    static std::unordered_map<std::string, MIX_Audio*> audio;
    static std::unordered_map<std::string, double> sfxLastPlayedTimes;

public:
    AudioManager();

    void loadAudio(const std::string& name, const char* path);

    static void playMusic(const std::string& name);
    static void pauseMusic();
    static void resumeMusic();
    static void stopMusic();

    static void playSfx(const std::string& name);
};