#include "PlayerShotFactory.h"

#include "Component.h"
#include "World.h"
#include "manager/AssetManager.h"
#include "manager/TextureManager.h"

class World;

void PlayerShotFactory::buildPlayerDanmaku(Entity& player, World& world, const Vector2D& playerPosition) {
    float frequency = 0.05f;
    float yOffset = 50;
    float fanSpread = 5;

    float smallShotBulletSpeed = 1200.0f;
    float largeShotBulletSpeed = 1200.0f;
    float smallFanBulletSpeed = 1200.0f;

    // TODO: balance these values
    float smallShotBulletDamage = 6.0f; // 0.03 * 4 bullets * 1/0.05 = 2.4 damage per second.
    float largeShotBulletDamage = 10.0f; // 0.05 * 1 bullet * 1/0.05 =  2 damage per second.
    float smallFanBulletDamage = 3.0f; // 0.015 * 4 bullets * 1/0.05 = 1.2 damage per second.

    // Small shot.
    auto& smallShotEntity(world.createEntity());
    std::vector<Vector2D> smallShotPositions = {
    	{-20, yOffset},
    	{-10, yOffset},
    	{10, yOffset},
    	{20, yOffset}
    };
    smallShotEntity.addComponent<Transform>(playerPosition, 180.0f, 1.0f);
    auto& smallShotSpawner = smallShotEntity.addComponent<LinearSpawner>(
		false,
		false,
		smallShotBulletSpeed,
		1.0f,
		smallShotPositions,
		frequency,
		[&world, smallShotBulletDamage](Vector2D position, Vector2D direction, float speed) {
			auto& e(world.createDeferredEntity());

			e.addComponent<Transform>(position, 0.0f, 1.0f);
			e.addComponent<Velocity>(direction, speed, false);

			SDL_Texture* tex = TextureManager::load("../asset/bullet-spritesheet.png");
			SDL_SetTextureAlphaMod(tex, 150);
			SDL_FRect src = {240, 80, 16, 16};
			SDL_FRect dest { position.x, position.y, 32, 32 };
			Vector2D pivotOffset = Vector2D(dest.w / 2.0f, dest.h / 2.0f);
			e.addComponent<Sprite>(tex, src, dest, RenderLayer::WorldBackground, pivotOffset);

			// TODO: adjust bullet collider
			auto& c = e.addComponent<CircleCollider>("player-shot");
			c.centerPosition = position;
			c.radius = 10;

			e.addComponent<PlayerShot>(smallShotBulletDamage);
		});

    // Large shot.
    auto& largeShotEntity(world.createEntity());
    std::vector<Vector2D> largeShotPositions = { {0, yOffset} };
    largeShotEntity.addComponent<Transform>(playerPosition, 180.0f, 1.0f);
    auto& largeShotSpawner = largeShotEntity.addComponent<LinearSpawner>(
		false,
		false,
		largeShotBulletSpeed,
		1.0f,
		largeShotPositions,
		frequency,
		[&world, largeShotBulletDamage](Vector2D position, Vector2D direction, float speed) {
			auto& e(world.createDeferredEntity());

			e.addComponent<Transform>(position, 0.0f, 1.0f);
			e.addComponent<Velocity>(direction, speed, false);

			SDL_Texture* tex = TextureManager::load("../asset/bullet-spritesheet.png");
			SDL_SetTextureAlphaMod(tex, 100);
			SDL_FRect src = {32, 96, 16, 16};
			SDL_FRect dest { position.x, position.y, 32, 32 };
			Vector2D pivotOffset = Vector2D(dest.w / 2.0f, dest.h / 2.0f);
			e.addComponent<Sprite>(tex, src, dest, RenderLayer::WorldBackground, pivotOffset);

			// TODO: adjust bullet collider
			auto& c = e.addComponent<CircleCollider>("player-shot");
			c.centerPosition = position;
			c.radius = 10;

			e.addComponent<PlayerShot>(largeShotBulletDamage);
		});

    // Small fan.
    auto& smallFanEntity(world.createEntity());
    std::vector<Vector2D> smallFanPositions = {
    		{-2 * fanSpread, -yOffset},
		{-1 * fanSpread, -yOffset},
		{1 * fanSpread, -yOffset},
		{2 * fanSpread, -yOffset}
    };
    smallFanEntity.addComponent<Transform>(playerPosition, 0.0f, 1.0f);
    auto& smallFanSpawner = smallFanEntity.addComponent<LinearSpawner>(
		false,
		true,
		smallFanBulletSpeed,
		1.0f,
		smallFanPositions,
		frequency,
		[&world, smallFanBulletDamage](Vector2D position, Vector2D direction, float speed) {
			auto& e(world.createDeferredEntity());

			e.addComponent<Transform>(position, 0.0f, 1.0f);
			e.addComponent<Velocity>(direction, speed, false);

			SDL_Texture* tex = TextureManager::load("../asset/bullet-spritesheet.png");
			SDL_SetTextureAlphaMod(tex, 150);
			SDL_FRect src = {240, 32, 16, 16};
			SDL_FRect dest { position.x, position.y, 32, 32 };
            Vector2D pivotOffset = Vector2D(dest.w / 2.0f, dest.h / 2.0f);
			e.addComponent<Sprite>(tex, src, dest, RenderLayer::WorldBackground, pivotOffset);

			// TODO: adjust bullet collider
			auto& c = e.addComponent<CircleCollider>("player-shot");
			c.centerPosition = position;
			c.radius = 10;

			e.addComponent<PlayerShot>(smallFanBulletDamage);
		});

	// Add shot spawners as children of player.
	auto& playerChildren = player.addComponent<Children>();
	playerChildren.children.emplace_back(&smallShotEntity);
	playerChildren.children.emplace_back(&largeShotEntity);
	playerChildren.children.emplace_back(&smallFanEntity);
};
