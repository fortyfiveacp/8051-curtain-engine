#pragma once
#include <iostream>
#include <string>

#include "Component.h"
#include "SDL3_mixer/SDL_mixer.h"

enum class SoundID;
enum class MusicID;

class AssetManager {
    static std::unordered_map<std::string, Animation> animations;
    static std::unordered_map<std::string, TTF_Font*> fonts;

    static Animation loadAnimationFromXML(const char* path);

    static std::unordered_map<SoundID, MIX_Audio*> sounds;
    static std::unordered_map<MusicID, MIX_Audio*> music;
public:
    static void loadFont(const std::string& name, const char* path, float fontSize);
    static TTF_Font* getFont(const std::string& name);

    static void loadAnimation(const std::string& clipname, const char* path);
    static const Animation& getAnimation(const std::string& clipName);

    static void loadSound(SoundID id, const char* path) {
        MIX_Audio* audio = MIX_LoadAudio(nullptr, path, true);
        if (!audio) std::cerr << "Failed to load SFX: " << path << " " << SDL_GetError() << std::endl;
        sounds[id] = audio;
    }

    static MIX_Audio* getSound(SoundID id) {
        return sounds[id];
    }

    static void loadMusic(MusicID id, const char* path) {
        MIX_Audio* audio = MIX_LoadAudio(nullptr, path, false);
        if (!audio) std::cerr << "Failed to load Music: " << path << " " << SDL_GetError() << std::endl;
        music[id] = audio;
    }

    static MIX_Audio* getMusic(MusicID id) {
        return music[id];
    }
};
