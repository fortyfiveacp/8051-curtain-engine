#include "ItemFactory.h"

#include "manager/TextureManager.h"

void ItemFactory::createItem(Entity& entity, ItemType type, Vector2D position) {
    auto& transform = entity.addComponent<Transform>(position, 0.0f, 1.0f);

    switch (type) {
        case LargePower:
            createLargePowerItem(entity, transform);
            break;
        case Point:
            createPointItem(entity, transform);
            break;
        case SmallPower:
            createSmallPowerItem(entity, transform);
            break;
        case Bomb:
            createBombItem(entity, transform);
            break;
        default:
            break;
    }
}

void ItemFactory::createBaseItem(Entity& entity, Transform transform, float textureX, float textureY) {
    SDL_Texture* tex = TextureManager::load("../asset/item-spritesheet.png");
    float texWidth = 16;
    float texHeight = 16;
    SDL_FRect src {textureX, textureY, texWidth, texHeight};
    SDL_FRect dst {transform.position.x, transform.position.y, 32, 32};
    entity.addComponent<Sprite>(tex, src, dst);

    // Items start with a "bounce" upwards when created.
    entity.addComponent<Velocity>(Vector2D(0, -1), 200.0f);
    entity.addComponent<ItemBounce>();

    auto& collider = entity.addComponent<Collider>("item");

    // Make the collider bigger so it's a bit easier to pick up.
    collider.rect.w = dst.w * 1.8f;
    collider.rect.h = dst.h * 1.8f;

    // Add offset to the collider to it's centered on the destination rect.
    collider.offset.x = (dst.w  - collider.rect.w) / 2.0f;
    collider.offset.y = (dst.h - collider.rect.h) / 2.0f;
}

void ItemFactory::createPointItem(Entity& entity, Transform transform) {
    createBaseItem(entity, transform, 16, 0);
    entity.addComponent<Item>(4500, Point);
}

void ItemFactory::createSmallPowerItem(Entity& entity, Transform transform) {
    createBaseItem(entity, transform, 0, 0);
    entity.addComponent<Item>(1, SmallPower);
}

void ItemFactory::createLargePowerItem(Entity& entity, Transform transform) {
    createBaseItem(entity, transform, 32, 0);
    entity.addComponent<Item>(5, LargePower);
}

void ItemFactory::createBombItem(Entity& entity, Transform transform) {
    createBaseItem(entity, transform, 48, 0);
    entity.addComponent<Item>(1, Bomb);
}