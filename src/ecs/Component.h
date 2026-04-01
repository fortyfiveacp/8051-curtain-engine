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
    float speed{};

    // If true, the direction is influenced by Transform.rotation.
    bool isLocalSpace{};
};

// Rotation over time, which modifies the rotation property of the Transform.
struct AngularVelocity {
    float rotationOverTime{};
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
};

struct TimedSpawner {
    float spawnInterval{};
    std::function<void()> spawnCallback{};
    float timer{};
};

struct RadialSpawner {
    // Whether to emit projectiles.
    bool isActive{};

    // The spawner's rotation speed.
    float rotationSpeed{};

    // The frequency (sec) that one burst of bullets is emitted.
    float frequency{};

    // Bullet emission speed.
    float bulletEmissionSpeed{};

    // Bullet emission angular velocity (rotation over time).
    float bulletEmissionAngularVelocity{};

    // Inner radius of bullet pattern emitters.
    float radius{};

    // Number of bullets in each burst.
    int bulletsPerBurst{};

    // Invoked when spawning, once per bullet, with param direction.
    std::function<void(Vector2D)> spawnCallback{};

    // The actual spawn countdown, which triggers spawn when it hits zero then resets.
    float spawnTimer{frequency};
};

struct LinearSpawner {
    // Whether to emit projectiles.
    bool isActive{};

    // Whether bullets radiate from emitter center after emission.
    bool isFanPattern;

    // Bullet emission speed.
    float bulletEmissionSpeed{};

    // Bullet speed multiplier relative to the distance between bullet and emitter center.
    float bulletEmissionSpeedMultiplier{};

    // List of positions for bullets to emit in the burst relative to emitter center.
    std::vector<Vector2D> bulletPositions{};

    // The frequency (sec) that one burst of bullets is emitted
    float frequency{};

    // Invoked when spawning, once per bullet, with params global position, direction, speed.
    std::function<void(Vector2D, Vector2D, float)> spawnCallback{};

    // The actual spawn countdown, which triggers spawn when it hits zero then resets.
    float spawnTimer{frequency};
};

// Our game state, might have multiple scenes.
struct SceneState {
    int coinsCollected = 0;
};

struct Health {
    int currentHealth{};
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
    PlayerPosition,
    FPSCounter,
    Static
};

struct Label {
    std::string text{};
    TTF_Font* font = nullptr;
    SDL_Color color{};
    LabelType type = LabelType::PlayerPosition; // Default to player position for tutorial.
    std::string textureCacheKey{};
    SDL_Texture* texture = nullptr;
    SDL_FRect dst{};
    bool visible = true;
    bool dirty = false;
};

struct FPSCounter {
    int frameCount = 0;
    float timer = 1.0f; // Default start timer at 1 so the first update isn't delayed by 1 second.
};

struct StageBackground {
    float baseWidth{};
    float baseHeight{};
    float scrollSpeedY = 100.0f;
    float offsetY = 0.0f;
    SDL_Texture* texture{};
};

struct PlayerTag{};
struct PauseMenuTag{};
struct ProjectileTag{};
