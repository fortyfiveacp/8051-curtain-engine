#pragma once
#include <memory>

#include "Component.h"

class TransformUtils {
    public:
    static void setPosition(const std::unique_ptr<Entity>& entity, Vector2D newPosition) {
        if (entity->hasComponent<Transform>()) {
            auto& t = entity->getComponent<Transform>();

            Vector2D displacement = newPosition - t.position;
            t.position = newPosition;

            // Update position of children Transforms.
            if (entity->hasComponent<Children>()) {
                auto&[children] = entity->getComponent<Children>();

                for (const auto& child : children) {
                    if (child->hasComponent<Transform>()) {
                        auto& childTransform = child->getComponent<Transform>();
                        childTransform.position += displacement;
                    }
                }
            }
        }
    }
};
