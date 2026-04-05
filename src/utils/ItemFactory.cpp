#include "ItemFactory.h"

#include "manager/TextureManager.h"

void ItemFactory::createItem(Entity& entity, ItemType type, Vector2D position) {
    switch (type) {
        case Point:
            createPointItem(entity, position);
            break;
        case LargePower:
            createLargePowerItem(entity, position);
            break;
        case SmallPower:
            createSmallPowerItem(entity, position);
            break;
        case Bomb:
            createBombItem(entity, position);
            break;
        default:
            break;
    }
}

void ItemFactory::createBaseItem(Entity& entity, Vector2D position, float textureX, float textureY) {
    auto& transform = entity.addComponent<Transform>(position, 0.0f, 1.0f);

    SDL_Texture* tex = TextureManager::load("../asset/item-spritesheet.png");
    float texWidth = 16;
    float texHeight = 16;
    SDL_FRect src {textureX, textureY, texWidth, texHeight};
    SDL_FRect dst {transform.position.x, transform.position.y, 32, 32};
    Vector2D pivotOffset = Vector2D(dst.w / 2.0f, dst.h / 2.0f);
    entity.addComponent<Sprite>(tex, src, dst, RenderLayer::World, pivotOffset);

    // Items start with a "bounce" upwards when created.
    entity.addComponent<Velocity>(Vector2D(0, -1), 220.0f);
    entity.addComponent<ItemBounce>();

    auto& collider = entity.addComponent<RectCollider>("item");

    // Make the collider bigger so it's a bit easier to pick up.
    collider.rect.w = dst.w * 1.8f;
    collider.rect.h = dst.h * 1.8f;

    // Add offset to the collider to it's centered on the destination rect.
    collider.offset.x = -collider.rect.w / 2.0f;
    collider.offset.y = -collider.rect.h / 2.0f;

    // Add projectile tag so items are destroyed when they go off-screen.
    entity.addComponent<ProjectileTag>();
}

void ItemFactory::createPointItem(Entity& entity, Vector2D position) {
    createBaseItem(entity, position, 16, 0);
    entity.addComponent<Item>(7500, Point);
}

void ItemFactory::createLargePowerItem(Entity& entity, Vector2D position) {
    createBaseItem(entity, position, 32, 0);
    entity.addComponent<Item>(5, LargePower);
}

void ItemFactory::createSmallPowerItem(Entity& entity, Vector2D position) {
    createBaseItem(entity, position, 0, 0);
    entity.addComponent<Item>(1, SmallPower);
}

void ItemFactory::createBombItem(Entity& entity, Vector2D position) {
    createBaseItem(entity, position, 48, 0);
    entity.addComponent<Item>(1, Bomb);
}