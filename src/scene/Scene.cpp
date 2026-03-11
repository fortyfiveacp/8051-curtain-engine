#include "../manager/AssetManager.h"
#include "Game.h"
#include "Scene.h"

Scene::Scene(SceneType sceneType, const char* sceneName, const char* mapPath, const int windowWidth,
const int windowHeight) : name(sceneName), type(sceneType) {

	if (sceneType == SceneType::MainMenu) {
		// Camera
		auto& cam = world.createEntity();
		cam.addComponent<Camera>();

		// Menu
		auto& menu(world.createEntity());
		auto menuTransform = menu.addComponent<Transform>(Vector2D(0, 0), 0.0f, 1.0f);

		SDL_Texture* texture = TextureManager::load("../asset/menu.png");
		SDL_FRect menuSrc {0, 0, (float)windowWidth, (float)windowHeight};
		SDL_FRect menuDst {menuTransform.position.x, menuTransform.position.y, menuSrc.w, menuSrc.h};
		menu.addComponent<Sprite>(texture, menuSrc, menuDst);
		return;
	}

	// Load our map
	world.getMap().load(mapPath, TextureManager::load("../asset/tileset.png"));
	for (auto& collider : world.getMap().colliders) {
		auto& e = world.createEntity();
		e.addComponent<Transform>(Vector2D(collider.rect.x, collider.rect.y), 0.0f, 1.0f);
		auto& c = e.addComponent<Collider>("no-wall");
		c.rect.x = collider.rect.x;
		c.rect.y = collider.rect.y;
		c.rect.w = collider.rect.w;
		c.rect.h = collider.rect.h;

		// Have a visual of the colliders
		SDL_Texture* tex = TextureManager::load("../asset/tileset.png");
		SDL_FRect colSrc {0, 32, 32, 32};
		SDL_FRect colDst {c.rect.x, c.rect.y, c.rect.w, c.rect.h};
		e.addComponent<Sprite>(tex, colSrc, colDst);
	}

	// Player
	// player = new GameObject("../asset/ball.png", 0, 0);

	// Add entities
	for (auto& itemCollider : world.getMap().itemColliders) {
		auto& e = world.createEntity();
		e.addComponent<Transform>(Vector2D(itemCollider.rect.x, itemCollider.rect.y), 0.0f, 1.0f);
		auto& c = e.addComponent<Collider>("item");
		c.rect.x = itemCollider.rect.x;
		c.rect.y = itemCollider.rect.y;
		c.rect.w = itemCollider.rect.w;
		c.rect.h = itemCollider.rect.h;

		SDL_Texture* itemTex = TextureManager::load("../asset/coin.png");
		SDL_FRect itemSrc {0, 0, 32, 32};
		SDL_FRect itemDst {c.rect.x, c.rect.y, 32, 32};
		e.addComponent<Sprite>(itemTex, itemSrc, itemDst);
	}

	auto& cam = world.createEntity();
	SDL_FRect camView{};
	camView.w = windowWidth; // width of the window
	camView.h = windowHeight; // height of the window
	cam.addComponent<Camera>(camView, world.getMap().width * 32.0f, world.getMap().height * 32.0f);

	auto& player (world.createEntity());
	auto& playerTransform = player.addComponent<Transform>(Vector2D(0, 0), 0.0f, 1.0f);
	player.addComponent<Velocity>(Vector2D(0.0f, 0.0f), 120.0f);

	Animation anim = AssetManager::getAnimation("player");
	player.addComponent<Animation>(anim);

	SDL_Texture* texture = TextureManager::load("../asset/animations/fox_anim.png");
	// SDL_FRect playerSrc {0, 0, 32, 44}; // for Mario
	SDL_FRect playerSrc = anim.clips[anim.currentClip].frameIndices[0];
	SDL_FRect playerDst {playerTransform.position.x, playerTransform.position.y, 64, 64};

	player.addComponent<Sprite>(texture, playerSrc, playerDst);

	auto& playerCollider = player.addComponent<Collider>("player");
	playerCollider.rect.w = playerDst.w;
	playerCollider.rect.h = playerDst.h;

	player.addComponent<PlayerTag>();
	player.addComponent<Health>(Game::gameState.playerHealth);

	auto& spawner(world.createEntity());
	Transform t = spawner.addComponent<Transform>(Vector2D(windowWidth / 2, windowHeight - 5), 0.0f, 1.0f);
	spawner.addComponent<TimedSpawner>(2.0f, [this, t] {
		// Create the projectile (birds)
		auto& e(world.createDeferredEntity());
		e.addComponent<Transform>(Vector2D(t.position.x, t.position.y), 0.0f, 1.0f);
		e.addComponent<Velocity>(Vector2D(0, -1), 100.0f);

		Animation anim = AssetManager::getAnimation("enemy");
		e.addComponent<Animation>(anim);

		SDL_Texture* tex = TextureManager::load("../asset/animations/bird_anim.png");
		SDL_FRect src { 0, 0, 32, 32 };
		SDL_FRect dest { t.position.x, t.position.y, 32, 32 };
		e.addComponent<Sprite>(tex, src, dest);

		Collider c = e.addComponent<Collider>("projectile");
		c.rect.w = dest.w;
		c.rect.h = dest.h;

		e.addComponent<ProjectileTag>();
	});

	// Add timeline object (experimental, this will actually spawn enemy convoys later)
	auto& timelineManager(world.createEntity());
	auto& debugTimeline = timelineManager.addComponent<Timeline>();

	debugTimeline.timeline.emplace_back(1.0, [] {
		std::cout << "Hello" << std::endl;
	});
	debugTimeline.timeline.emplace_back(2.0, [] {
		std::cout << "World" << std::endl;
	});
	debugTimeline.timeline.emplace_back(4.0, [] {
		std::cout << "Welcome ";
	});
	debugTimeline.timeline.emplace_back(4.5, [] {
		std::cout << "to ";
	});
	debugTimeline.timeline.emplace_back(5.0, [] {
		std::cout << "Touhou ";
	});
	debugTimeline.timeline.emplace_back(5.5, [] {
		std::cout << "Miko ";
	});
	debugTimeline.timeline.emplace_back(6.0, [] {
		std::cout << "Warfare";
	});
	debugTimeline.timeline.emplace_back(6.0, [] {
		std::cout << "!!" << std::endl;
	});

	// Add scene state
	auto& state(world.createEntity());
	state.addComponent<SceneState>();
}
