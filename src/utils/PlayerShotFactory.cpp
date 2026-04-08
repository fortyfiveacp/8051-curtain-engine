#include "Component.h"
#include "World.h"

class World;

class PlayerShotFactory {
    void buildPlayerDanmaku(Entity& player, World& world, Vector2D& playerPosition, const PlayerShot& playerShot) {
	    // Small shot.
    	auto& smallShotEntity(world.createEntity());
    	std::vector<Vector2D> smallShotPositions = {
    		{-20, playerShot.yOffset},
    		{-10, playerShot.yOffset},
    		{10, playerShot.yOffset},
    		{20, playerShot.yOffset}
    	};
    	smallShotEntity.addComponent<Transform>(playerPosition, 180.0f, 1.0f);
    	auto& smallShotSpawner = smallShotEntity.addComponent<LinearSpawner>(
			false,
			false,
			playerShot.smallShotBulletSpeed,
			1.0f,
			smallShotPositions,
			playerShot.frequency,
			[this](Vector2D position, Vector2D direction, float speed) {
				// TODO: small shot
			});

    	// Large shot.
    	auto& largeShotEntity(world.createEntity());
    	std::vector<Vector2D> largeShotPositions = { {0, playerShot.yOffset} };
    	largeShotEntity.addComponent<Transform>(playerPosition, 180.0f, 1.0f);
    	auto& largeShotSpawner = largeShotEntity.addComponent<LinearSpawner>(
			false,
			false,
			playerShot.largeShotBulletSpeed,
			1.0f,
			largeShotPositions,
			playerShot.frequency,
			[this](Vector2D position, Vector2D direction, float speed) {
				// TODO: large shot
			});

    	// Small fan.
    	auto& smallFanEntity(world.createEntity());
    	std::vector<Vector2D> smallFanPositions = {
    		{-20, playerShot.yOffset},
			{-10, playerShot.yOffset},
			{10, playerShot.yOffset},
			{20, playerShot.yOffset}
    	};
    	smallFanEntity.addComponent<Transform>(playerPosition, 180.0f, 1.0f);
    	auto& smallFanSpawner = player.addComponent<LinearSpawner>(
			false,
			false,
			playerShot.smallShotBulletSpeed,
			1.0f,
			smallShotPositions,
			playerShot.frequency,
			[this](Vector2D position, Vector2D direction, float speed) {
				// TODO: fan shot
			});

		// Add shot spawners as children of player.
		auto& playerChildren = player.addComponent<Children>();
		playerChildren.children.emplace_back(&smallShotEntity);
		playerChildren.children.emplace_back(&largeShotEntity);
		playerChildren.children.emplace_back(&smallFanEntity);
    }
};
