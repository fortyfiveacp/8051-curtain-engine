#pragma once
#include "Entity.h"

enum class EventType {
    Collision,
    PlayerAction,
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

enum class UIInteractionState {Pressed, Released, Selected};

struct UIInteractionEvent : BaseEvent {
    Entity* entity = nullptr;
    UIInteractionState state{};
    UIInteractionEvent(Entity* entity, UIInteractionState state) : entity(entity), state(state) {
        type = EventType::UIInteraction;
    }
};
