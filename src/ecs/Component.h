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

    // If true, the direction is influenced by Transform.rotation.
    bool isLocalSpace{};

    float currentSpeed = baseSpeed;
};

// Rotation over time, which modifies the rotation property of the Transform.
struct AngularVelocity {
    float rotationOverTime{};
};

// Sets the rotation property of the Transform to point toward a different target Transform.
struct LookAtRotator {
    // The target so that the rotator will point local space DOWN toward the target.
    Transform& target;

    // Offset to the target in degrees.
    float offsetDegrees{};
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
    Vector2D pivotOffset = Vector2D(0, 0);
    bool visible = true;
};

struct RectCollider {
    std::string tag;
    SDL_FRect rect{};
    Vector2D offset{}; // Offset for collider positioning relative to the entity's transform.
    bool enabled = true;
};

struct CircleCollider {
    std::string tag;
    float radius{}; // Radius of the circle used for detecting collisions.
    Vector2D centerPosition{}; // Position of the collider (this is the actual position, updated by collision system).
    Vector2D offset{}; // Offset for collider center relative to the entity's transform.
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
    int coinsCollected = 0; // TODO: do we actually even need this?
};

struct PlayerStats {
    int currentHiScore{};
    int currentScore{};
    int currentHealth{};
    int currentBombs{};
    int currentPower{};
    int currentGraze{};
    int currentPoint{};
    static constexpr int MAX_SCORE = 999999999;
    static constexpr int MAX_HEALTH = 8;
    static constexpr int MAX_BOMBS = 8;
    static constexpr int MAX_POWER = 400;
};

struct PlayerBomb {
    float timer = 0.0f;
    float duration = 3.0f;
    float damage = 1.0f; // TODO: Test damage values once boss and health are implemented.
};

struct PlayerBombAbility {
    float cooldown = 3.0f;
    float cooldownTimer = 0.0f;
    bool active = false;
};

struct DeathBombState {
    bool isHit = false;
    float windowDuration = 0.1f;
    float timer = 0.0f;
};

struct SelectableUI {
    std::function<void()> onPressed{};
    std::function<void()> onReleased{};
    std::function<void()> onSelect{};
    bool selected = false;

    // The selectable UI elements are a doubly linked list.
    // Upon some reflection this may or may not be best practice in terms of ECS design.
    Entity* next = nullptr;
    Entity* previous = nullptr;
};

struct Toggleable {
    std::function<void()> toggle;
    bool enabled = true;
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


struct PathPoint {
    Vector2D position;
    float hoverTime = 0.0f;
};

struct Path {
    std::vector<PathPoint> points{};
};

struct PathFollower {
    int pathId{};
    float distance{};
    float speed{};
    bool active = true;
    float currentHoverTimer = 0.0f;
    int lastPointReached = -1;
};

enum class EnemyType {
    SmallBlueFairy,
    SmallRedFairy,
    LargeFairy,
    Boss
};

enum class BulletType {
    Circle,
    LargeOrb
};

enum class DanmakuType {
    Radial,
    Linear
};

struct DanmakuPattern {
    bool hasPattern = false;
    DanmakuType danmakuType = DanmakuType::Radial;
    BulletType bulletType = BulletType::Circle;

    float startTime{};
    float endTime{};
    float frequency{};
    float bulletSpeed{};

    // Radial Specific.
    int bulletsPerBurst{};
    float rotationSpeed{};
    float bulletAngularVel{};
    float radius{};

    // Linear Specific.
    bool isFanPattern = false;
    bool shouldTargetPlayer = true;
    float speedMultiplier = 1.0f;
    std::vector<Vector2D> bulletPositions{};
};

struct Convoy {
    EnemyType enemyType{};
    int pathId{};
    int numEnemies{};
    float speed{};
    float spawnInterval{};
    float timer = 0.0f;

    DanmakuPattern danmakuPattern{};
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
    SDL_Color outlineColor {0, 0, 0, 255};
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
    Bomb,
    BossPhase
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
    float duration = 4.0f; // Note that the invincibility duration ticks down during respawn delay.
    float timer = 0.0f;
    float flickerFrequency = 10.0f; // The number of sprite flickers per second when invincible.
    bool active = false;
};

enum ItemType {
    Point,
    LargePower,
    SmallPower,
    Bomb
};

struct Item {
    int value{};
    ItemType type{};
};

// Note that bounces require a velocity component with an upward direction at the start.
struct ItemBounce {
    float bounceDuration = 0.75f; // Duration of upwards movement when the item is created.
    float timer = bounceDuration;
    bool isBouncing = true;
};

struct PlayerRespawn {
    Vector2D playerStartingPosition{};
    bool isRespawning = false;
    float respawnDelay = 0.5f;
    float timer = 0.0f;
};

struct Fade {
    float fadeDuration = 1.5f;
    float fadeDelayDuration = 0.0f;
    Uint8 startingAlpha = 0; // Initial alpha as the fade starts.
    Uint8 endingAlpha = 255; // Alpha at the end of the fade, default values do a fade in.
    float delayTimer = 0.0f;
    float durationTimer = 0.0f;
    bool isFading = false;
};

struct Boss {
    std::string bossName{};
    int maxHealth{};
    int currentHealth{};
    int phasesLeft{};
};

struct BossHealthBar {
    float fullDstWidth{};
    float initializationDuration = 1.0f;
    float timer = 0.0f;
    bool isInitialized = false;
};

struct BossTracker {
    float minX{};
    float maxX{};
    bool isInitialized = false;
};

struct PlayerTag{};
struct PauseMenuTag{};
struct ContinueGameMenuTag{};
struct WinGameMenuTag{};
struct ProjectileTag{};
struct EnemyTag{};
