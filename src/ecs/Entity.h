#pragma once
#include <array>
#include <bitset>

#include "ComponentType.h"

// Create a const that is evaluated at compile time (constexpr)
// 32 components fits perfectly in a 32-bit integer
constexpr std::size_t MAX_COMPONENTS = 32;

// Bitset is an array with only 0 or 1 as values
// bitset[0] = 1  entity has component 0
// Bit operations are fast and work well for checking each frame.
using ComponentBitSet = std::bitset<MAX_COMPONENTS>;

// Fixed sized array where each index is a component (void*).
// The index corresponds to the ComponentTypeID
// We use a fixed number of components so there is no dynamic allocation for the array (efficient)
using ComponentArray = std::array<void*, MAX_COMPONENTS>;

class Entity {
    bool active = true;
    ComponentArray componentArray{};
    ComponentBitSet componentBitSet{};

public:
    bool isActive() {
        return active;
    }

    void destroy() {
        active = false;
    }

    // Templates need to be implemented in header.
    // entity.hasComponent<Position>() returns true or false
    template <typename T>
    bool hasComponent() {
        return componentBitSet[getComponentTypeID<T>()];
    }

    // Template that can receive multiple arguments
    // entity.addComponent<Position>(4, 5)
    // entity.addComponent<Health>(100)
    template <typename T, typename... mArgs>
    T& addComponent(mArgs&&...args) {
        // Implements perfect forwarding.
        // Everything is not treated as an lvalue.
        // More efficient with moves and temporaries
        T* component(new T{std::forward<mArgs>(args)...});

        // Add the new component to the array.
        componentArray[getComponentTypeID<T>()] = component;

        // Update the bitset to true, it does have this component.
        componentBitSet[getComponentTypeID<T>()] = true;

        return *component;
    }

    template<typename T>
    T& getComponent() {
        auto ptr(componentArray[getComponentTypeID<T>()]);
        return *static_cast<T*>(ptr);
    }

    template <typename T>
    void deactivateComponent() {
        componentBitSet[getComponentTypeID<T>()] = false;
    }
};
