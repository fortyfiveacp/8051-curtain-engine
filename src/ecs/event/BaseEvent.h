#pragma once
#include "Entity.h"

enum class EventType {
    Collision,
    PlayerAction,
    Audio,
    UIInteraction
};

struct BaseEvent {
    EventType type{};
    virtual ~BaseEvent() = default; // In C++, always make the destructor virtual from a class you are deriving from.
};

enum class CollisionState { Enter, Stay, Exit };

struct CollisionEvent : BaseEvent {
    Entity* entityA = nullptr;
    Entity* entityB = nullptr;
    CollisionState state{};
    CollisionEvent(Entity* entityA, Entity* entityB, CollisionState state) : entityA(entityA), entityB(entityB), state(state) {
        type = EventType::Collision;
    }
};

enum class PlayerAction { Attack, Interact, UseItem, Jump };

struct PlayerActionEvent : BaseEvent {
    Entity* player = nullptr;
    PlayerAction action{};
    PlayerActionEvent(Entity* player, PlayerAction action) : player(player), action(action) {
        type = EventType::PlayerAction;
    }
};

enum class SoundID {
    PlayerShot
};

enum class MusicID {
    StageTheme
};

struct AudioEvent : BaseEvent {
    SoundID sound;

    AudioEvent(SoundID sound) : sound(sound) {
        type = EventType::Audio;
    }
};

// Might not be needed.
enum class MusicAction {
    Play,
    Stop
};

struct MusicEvent : BaseEvent {
    MusicID music;
    MusicAction action;
    bool loop;

    MusicEvent(MusicID music, MusicAction action, bool loop = true) : music(music), action(action), loop(loop) {
        type = EventType::Audio;
    }
};
enum class UIInteractionState {Pressed, Released, Selected};

struct UIInteractionEvent : BaseEvent {
    Entity* entity = nullptr;
    UIInteractionState state{};
    UIInteractionEvent(Entity* entity, UIInteractionState state) : entity(entity), state(state) {
        type = EventType::UIInteraction;
    }
};
