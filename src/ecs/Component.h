#pragma once
#include <functional>

#include "utils/Vector2D.h"
#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"

#include <string>
#include <unordered_map>

#include "AnimationClip.h"
#include "Entity.h"

struct Transform {
    Vector2D position{};
    float rotation{};
    float scale{};
    Vector2D oldPosition{};
};

// Direction and speed.
struct Velocity {
    Vector2D direction{};
    float baseSpeed{};
    float currentSpeed = baseSpeed;
};

enum class RenderLayer {
    Background,
    World,
    UI
};

struct Sprite {
    SDL_Texture* texture = nullptr;
    SDL_FRect src{};
    SDL_FRect dst{};
    RenderLayer renderLayer = RenderLayer::World;
    bool visible = true;
};

struct Collider {
    std::string tag;
    SDL_FRect rect{};
    Vector2D offset{}; // Offset for collider positioning relative to the entity's transform.
    bool enabled = true;
};

struct Animation {
    std::unordered_map<std::string, AnimationClip> clips{};
    std::string currentClip{};
    float time{}; // Time is accumulated for the current frame.
    int currentFrame{}; // Index of the current frame in the clip.
    float speed = 0.1f; // Time per frame.
};

struct Camera {
    SDL_FRect view;
    float worldWidth;
    float worldHeight;
    float outOfViewPadding{}; // The amount of extra space projectiles can move out of view before being destroyed.
};

struct TimedSpawner {
    float spawnInterval{};
    std::function<void()> spawnCallback{};
    float timer{};
};

// Our game state, might have multiple scenes.
struct SceneState {
    int coinsCollected = 0;
};

struct PlayerStats {
    int currentHealth{};
    int currentBombs{};
    Vector2D playerStartingPosition{};
    int currentHiScore{};
    int currentScore{};
    int currentPower{};
    int currentGraze{};
    int currentPoint{};
};

struct SelectableUI {
    std::function<void()> onPressed{};
    std::function<void()> onReleased{};
    std::function<void()> onSelect{};
    bool selected = false;

    // The selectable UI elements are a doubly linked list.
    SelectableUI* next = nullptr;
    SelectableUI* previous = nullptr;
};

struct Toggleable {
    std::function<void()> toggle;
};

struct Parent {
    Entity* parent = nullptr;
};

struct Children {
    std::vector<Entity*> children{};
};

// Controls pre-scripted events at specific times.
struct Timeline {
    float currentTime = 0;

    // Timeline elements have a float (time to trigger the action) and function (the action to trigger).
    std::vector<std::pair<float, std::function<void()>>> timeline{};
};

enum class LabelType {
    FPSCounter,
    Static,
    HiScore,
    Score,
    Health,
    Bomb,
    Power,
    Graze,
    Point
};

struct Label {
    std::string text{};
    TTF_Font* font = nullptr;
    SDL_Color color{};
    LabelType type = LabelType::Static;
    std::string textureCacheKey{};
    SDL_Texture* texture = nullptr;
    SDL_FRect dst{};
    bool visible = true;
    bool dirty = true;
};

struct FPSCounter {
    int frameCount = 0;
    float timer = 1.0f; // Default start timer at 1 so the first update isn't delayed by 1 second.
};

struct StageBackground {
    float scrollSpeedY = 100.0f;
    float offsetY = 0.0f;
    SDL_Texture* texture{};
};

enum class IconCounterType {
    Health,
    Bomb
};

struct IconCounter {
    int maxNumber{};
    SDL_Texture* texture = nullptr;
    IconCounterType type = IconCounterType::Health;
    int currentNumber{};
    bool visible = true;
    bool dirty = true;
};

struct KeyboardInput {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool focus = false;
    float focusMultiplier = 0.5f;
    bool shoot = false;
    bool bomb = false;
};

struct InvincibilityFrames {
    float duration = 4.0f;
    float timer = 0.0f;
    float flickerFrequency = 10.0f; // The number of sprite flickers per second when invincible.
    bool active = false;
};

struct PlayerTag{};
struct PauseMenuTag{};
struct ProjectileTag{};
