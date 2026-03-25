#pragma once
#include <functional>

#include "utils/Vector2D.h"
#include "SDL3/SDL_render.h"

#include <string>
#include <unordered_map>

#include "AnimationClip.h"

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

struct Sprite {
    SDL_Texture* texture = nullptr;
    SDL_FRect src{};
    SDL_FRect dst{};
};

struct Collider {
    std::string tag;
    SDL_FRect rect{};
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

    // Duration of spawner. Spawner turns off when duration expires.
    float duration{};

    // Delay before starting spawns.
    float delay{};

    // Number of bullets in each burst.
    int bulletsPerBurst{};

    // Invoked when spawning, once per bullet, with param direction.
    std::function<void(Vector2D)> spawnCallback{};

    // The actual spawn countdown, which triggers spawn when it hits zero then resets.
    float spawnTimer{frequency};

    // Time since the spawner was first updated. Used for timing emission start and end.
    float lifetime{};
};

struct LinearSpawner {
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

    // Duration of spawner. Spawner turns off when duration expires.
    float duration{};

    // Delay before starting spawns.
    float delay{};

    // Invoked when spawning, once per bullet, with params global position, direction, speed.
    std::function<void(Vector2D, Vector2D, float)> spawnCallback{};

    // The actual spawn countdown, which triggers spawn when it hits zero then resets.
    float spawnTimer{frequency};

    // Time since the spawner was first updated. Used for timing emission start and end.
    float lifetime{};
};

// Our game state, might have multiple scenes.
struct SceneState {
    int coinsCollected = 0;
};

struct Health {
    int currentHealth{};
};

// Controls pre-scripted events at specific times.
struct Timeline {
    float currentTime = 0;

    // Timeline elements have a float (time to trigger the action) and function (the action to trigger).
    std::vector<std::pair<float, std::function<void()>>> timeline{};
};

struct PlayerTag{};
struct ProjectileTag{};
