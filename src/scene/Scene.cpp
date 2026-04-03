#include "../manager/AssetManager.h"
#include "Game.h"
#include "Scene.h"
#include "ItemFactory.h"
#include "StageUtils.h"
#include "../manager/AudioManager.h"

Scene::Scene(SceneType sceneType, const char* sceneName, const char* mapPath, const int windowWidth,
             const int windowHeight) : name(sceneName), type(sceneType) {

	if (sceneType == SceneType::MainMenu) {
		initMainMenu(windowWidth, windowHeight);
		return;
	}

	initGameplay(mapPath, windowWidth, windowHeight);
}

void Scene::initMainMenu(int windowWidth, int windowHeight) {
	// Camera.
	auto& cam = world.createEntity();
	cam.addComponent<Camera>();

	// Menu.
	auto& menu(world.createEntity());
	auto menuTransform = menu.addComponent<Transform>(Vector2D(0, 0), 0.0f, 1.0f);

	SDL_Texture* texture = TextureManager::load("../asset/main-menu-scaled.png");
	SDL_FRect menuSrc {0, 0, (float)windowWidth, (float)windowHeight};
	SDL_FRect menuDst {menuTransform.position.x, menuTransform.position.y, menuSrc.w, menuSrc.h};
	menu.addComponent<Sprite>(texture, menuSrc, menuDst, RenderLayer::Background);

	// FPS counter.
	auto& fpsCounter = UIUtils::createLabel(world, windowWidth - 170, windowHeight - 40,
		{240, 240, 240, 255}, "pop1", "0.000fps", "fpsCounter", LabelType::FPSCounter);
	fpsCounter.addComponent<FPSCounter>();
}

void Scene::initGameplay(const char* mapPath, int windowWidth, int windowHeight) {
	// Subscribe to event for pausing the game.
	world.getEventManager().subscribe([this](const BaseEvent& e) {
		if (e.type != EventType::Pause) {
			return;
		}

		const auto& pauseEvent = dynamic_cast<const PauseEvent&>(e);
		isPaused = pauseEvent.isPaused;
	});

	// Subscribe to event for debugging the game.
	world.getEventManager().subscribe([this](const BaseEvent& e) {
		if (e.type != EventType::Debug) {
			return;
		}

		const auto& debugEvent = dynamic_cast<const DebugEvent&>(e);
		isDebugging = debugEvent.isDebugging;
	});


	SDL_Texture* backgroundTex = TextureManager::load("../asset/background.png");
	float texWidth = backgroundTex->w;
	float texHeight = backgroundTex->h;

	// Calculate how many times to repeat background image.
	int cols = (windowWidth + texWidth - 1) / texWidth;
	int rows = (windowHeight + texHeight - 1) / texHeight;

	// Create the tiled background.
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			auto& e = world.createEntity();
			SDL_FRect src {0, 0, texWidth, texHeight};
			SDL_FRect destRect = {static_cast<float>(x) * texWidth, static_cast<float>(y) * texHeight, texWidth, texHeight};
			e.addComponent<Transform>(Vector2D(x * texWidth, y * texHeight), 0.0f, 1.0f);
			e.addComponent<Sprite>(backgroundTex, src, destRect, RenderLayer::Background);
		}
	}

	// Create stage.
	float stageWidth = StageUtils::CalculateStageWidth(windowWidth);
	float stageHeight = StageUtils::CalculateStageHeight(windowHeight);
	float backgroundSpeed = 60.0f;
	float foregroundSpeed = backgroundSpeed + backgroundSpeed * 0.25f;

	// Create backgrounds.
	// The backgrounds are 1 pixel taller to make an overlap that hides the seam between backgrounds.
	StageUtils::createStageBackground(world, stageWidth, stageHeight + 1, 0, backgroundSpeed, "../asset/stage1.png");
	StageUtils::createStageBackground(world, stageWidth, stageHeight + 1, -stageHeight, backgroundSpeed, "../asset/stage1.png");

	// Create foregrounds.
	StageUtils::createStageBackground(world, stageWidth, stageHeight, 0, foregroundSpeed, "../asset/foreground1.png");
	StageUtils::createStageBackground(world, stageWidth, stageHeight, -stageHeight, foregroundSpeed, "../asset/foreground1.png");

	// TODO: purge unused systems.
	// Load our map.
	// world.getMap().load(mapPath, TextureManager::load("../asset/tileset.png"));
	// for (auto& collider : world.getMap().colliders) {
	// 	auto& e = world.createEntity();
	// 	e.addComponent<Transform>(Vector2D(collider.rect.x, collider.rect.y), 0.0f, 1.0f);
	// 	auto& c = e.addComponent<Collider>("no-wall");
	// 	c.rect.x = collider.rect.x;
	// 	c.rect.y = collider.rect.y;
	// 	c.rect.w = collider.rect.w;
	// 	c.rect.h = collider.rect.h;
	//
	// 	// Have a visual of the colliders
	// 	SDL_Texture* tex = TextureManager::load("../asset/tileset.png");
	// 	SDL_FRect colSrc {0, 32, 32, 32};
	// 	SDL_FRect colDst {c.rect.x, c.rect.y, c.rect.w, c.rect.h};
	// 	e.addComponent<Sprite>(tex, colSrc, colDst);
	// }

	// Player.
	// player = new GameObject("../asset/ball.png", 0, 0);

	// Add entities.
	// for (auto& itemCollider : world.getMap().itemColliders) {
	// 	auto& e = world.createEntity();
	// 	e.addComponent<Transform>(Vector2D(itemCollider.rect.x, itemCollider.rect.y), 0.0f, 1.0f);
	// 	auto& c = e.addComponent<Collider>("item");
	// 	c.rect.x = itemCollider.rect.x;
	// 	c.rect.y = itemCollider.rect.y;
	// 	c.rect.w = itemCollider.rect.w;
	// 	c.rect.h = itemCollider.rect.h;
	//
	// 	SDL_Texture* itemTex = TextureManager::load("../asset/coin.png");
	// 	SDL_FRect itemSrc {0, 0, 32, 32};
	// 	SDL_FRect itemDst {c.rect.x, c.rect.y, 32, 32};
	// 	e.addComponent<Sprite>(itemTex, itemSrc, itemDst);
	// }

	auto& cam = world.createEntity();
	SDL_FRect camView {0, 0, stageWidth, stageHeight};
	float outOfViewPadding = 100.0f;
	cam.addComponent<Camera>(camView, stageWidth, stageHeight, outOfViewPadding);

	// Create the player.
	auto& player (world.createEntity());
	player.addComponent<Velocity>(Vector2D(0.0f, 0.0f), 400.0f);

	Animation anim = AssetManager::getAnimation("player");
	player.addComponent<Animation>(anim);

	SDL_Texture* texture = TextureManager::load("../asset/animations/reimu_anim.png");
	SDL_FRect playerSrc = anim.clips[anim.currentClip].frameIndices[0];

	float scaledPlayerWidth = playerSrc.w * 1.75f;
	float scaledPlayerHeight = playerSrc.h * 1.75f;
	float playerStartingX = stageWidth / 2 - scaledPlayerWidth / 2;
	float playerStartingY = stageHeight - scaledPlayerHeight;
	auto& playerTransform = player.addComponent<Transform>(Vector2D(playerStartingX, playerStartingY), 0.0f, 1.0f);
	SDL_FRect playerDst {playerTransform.position.x, playerTransform.position.y, scaledPlayerWidth, scaledPlayerHeight};

	player.addComponent<Sprite>(texture, playerSrc, playerDst);

	auto& playerCollider = player.addComponent<Collider>("player");

	// Make the collider a square with side lengths of 1/8th the width of the player destination rect.
	playerCollider.rect.w = playerDst.w / 8;
	playerCollider.rect.h = playerDst.w / 8;

	// Add offset to the collider to it's centered on the player destination rect.
	playerCollider.offset.x = (playerDst.w  - playerCollider.rect.w) / 2.0f;
	playerCollider.offset.y = (playerDst.h - playerCollider.rect.h) / 2.0f;

	player.addComponent<PlayerTag>();
	player.addComponent<KeyboardInput>();
	player.addComponent<InvincibilityFrames>();
	player.addComponent<PlayerStats>(Game::gameState.playerHealth, Game::gameState.playerBombs,
		Vector2D(playerStartingX, playerStartingY));

	// TODO: purge.
	auto& spawner(world.createEntity());
	Transform t = spawner.addComponent<Transform>(Vector2D(stageWidth - 150, stageHeight - 5), 0.0f, 1.0f);
	spawner.addComponent<TimedSpawner>(2.0f, [this, t] {
		// Create the projectile (birds).
		auto& e(world.createDeferredEntity());
		e.addComponent<Transform>(Vector2D(t.position.x, t.position.y), 0.0f, 1.0f);
		e.addComponent<Velocity>(Vector2D(0, -1), 100.0f);

		Animation anim = AssetManager::getAnimation("redFairy");
		e.addComponent<Animation>(anim);

		SDL_Texture* tex = TextureManager::load("../asset/animations/small_fairies_anim.png");
		SDL_FRect src = {0, 31, 32, 31};
		SDL_FRect dest { t.position.x, t.position.y, 32, 31 };
		e.addComponent<Sprite>(tex, src, dest);

		auto& c = e.addComponent<Collider>("projectile");
		c.rect.w = dest.w / 1.5;
		c.rect.h = dest.h / 1.5;

		c.offset.x = (dest.w  - c.rect.w) / 2.0f;
		c.offset.y = (dest.h - c.rect.h) / 2.0f;

		e.addComponent<ProjectileTag>();
	});

	// Test spawners for items. TODO: remove when no longer needed.
	auto& pointSpawner(world.createEntity());
	Transform pointSpawnerTransform = pointSpawner.addComponent<Transform>(Vector2D(50, 250), 0.0f, 1.0f);
	pointSpawner.addComponent<TimedSpawner>(3.0f, [this, pointSpawnerTransform] {
		auto& itemEntity(world.createDeferredEntity());
		ItemFactory::createItem(itemEntity, Point, pointSpawnerTransform.position);
	});

	auto& smallPowerSpawner(world.createEntity());
	Transform smallPowerSpawnerTransform = smallPowerSpawner.addComponent<Transform>(Vector2D(125, 250), 0.0f, 1.0f);
	smallPowerSpawner.addComponent<TimedSpawner>(3.0f, [this, smallPowerSpawnerTransform] {
		auto& itemEntity(world.createDeferredEntity());
		ItemFactory::createItem(itemEntity, SmallPower, smallPowerSpawnerTransform.position);
	});

	auto& largePowerSpawner(world.createEntity());
	Transform largePowerSpawnerTransform = largePowerSpawner.addComponent<Transform>(Vector2D(200, 250), 0.0f, 1.0f);
	largePowerSpawner.addComponent<TimedSpawner>(3.0f, [this, largePowerSpawnerTransform] {
		auto& itemEntity(world.createDeferredEntity());
		ItemFactory::createItem(itemEntity, LargePower, largePowerSpawnerTransform.position);
	});

	auto& bombSpawner(world.createEntity());
	Transform bombSpawnerTransform = bombSpawner.addComponent<Transform>(Vector2D(275, 250), 0.0f, 1.0f);
	bombSpawner.addComponent<TimedSpawner>(3.0f, [this, bombSpawnerTransform] {
		auto& itemEntity(world.createDeferredEntity());
		ItemFactory::createItem(itemEntity, Bomb, bombSpawnerTransform.position);
	});

	// Radial danmaku spawner.
	auto& radialDanmaku(world.createEntity());
	auto radialDanmakuTransform = radialDanmaku.addComponent<Transform>(Vector2D(400, 400), 0.0f, 1.0f);

	float rotationSpeed = 50.0f;
	radialDanmaku.addComponent<AngularVelocity>(rotationSpeed);

	float frequency = 0.17f;
	float bulletEmissionSpeed = 150.0f;
	float bulletEmissionAngularVelocity = 20.0f;
	float radius = 30.0f;
	float duration = 10.0f;
	float delay = 2.0f;
	int bulletsPerBurst = 6;

	// RadialSpawner component takes a callback which spawns individual bullets.
	auto& radialSpawner = radialDanmaku.addComponent<RadialSpawner>(false, rotationSpeed, frequency, bulletEmissionSpeed, bulletEmissionAngularVelocity,
		radius, bulletsPerBurst,
		[this, radialDanmakuTransform, bulletEmissionSpeed, bulletEmissionAngularVelocity, radius](Vector2D direction) {
			auto& e(world.createDeferredEntity());

			Vector2D bulletSpawnPositionOffset = direction * radius;

			e.addComponent<Transform>(radialDanmakuTransform.position + bulletSpawnPositionOffset, 0.0f, 1.0f);
			e.addComponent<Velocity>(direction, bulletEmissionSpeed, true);

			e.addComponent<AngularVelocity>(bulletEmissionAngularVelocity);

			Animation anim = AssetManager::getAnimation("redFairy");
			e.addComponent<Animation>(anim);

			SDL_Texture* tex = TextureManager::load("../asset/animations/small_fairies_anim.png");
			SDL_FRect src = {0, 31, 32, 31};
			SDL_FRect dest { radialDanmakuTransform.position.x, radialDanmakuTransform.position.y, 32, 31 };
			e.addComponent<Sprite>(tex, src, dest);

			auto& c = e.addComponent<Collider>("projectile");
			c.rect.w = dest.w / 1.5;
			c.rect.h = dest.h / 1.5;

			c.offset.x = (dest.w  - c.rect.w) / 2.0f;
			c.offset.y = (dest.h - c.rect.h) / 2.0f;

			e.addComponent<ProjectileTag>();
		});

	// Linear danmaku spawner.
	auto& linearDanmaku(world.createEntity());
	linearDanmaku.addComponent<Transform>(Vector2D(400, 400), 0.0f, 1.0f);
	linearDanmaku.addComponent<LookAtRotator>(playerTransform, 0.0f);

	bool isFanPattern = false;
	float bulletEmissionSpeedMultiplier = 1.0f;

	std::vector<Vector2D> bulletSpawnPositions;
	bulletSpawnPositions.emplace_back(0, 30);
	bulletSpawnPositions.emplace_back(0, 40);
	bulletSpawnPositions.emplace_back(0, 50);
	bulletSpawnPositions.emplace_back(10, 20);
	bulletSpawnPositions.emplace_back(-10, 20);
	bulletSpawnPositions.emplace_back(15, 20);
	bulletSpawnPositions.emplace_back(-15, 20);

	auto& linearSpawner = linearDanmaku.addComponent<LinearSpawner>(false, isFanPattern, bulletEmissionSpeed, bulletEmissionSpeedMultiplier,
		bulletSpawnPositions, frequency,
		[this](Vector2D position, Vector2D direction, float speed) {
			auto& e(world.createDeferredEntity());

			e.addComponent<Transform>(position, 0.0f, 1.0f);
			e.addComponent<Velocity>(direction, speed, false);

			Animation anim = AssetManager::getAnimation("blueFairy");
			e.addComponent<Animation>(anim);

			SDL_Texture* tex = TextureManager::load("../asset/animations/small_fairies_anim.png");
			SDL_FRect src = {0, 31, 32, 31};
			SDL_FRect dest { position.x, position.y, 32, 31 };
			e.addComponent<Sprite>(tex, src, dest);

			auto& c = e.addComponent<Collider>("projectile");
			c.rect.w = dest.w / 1.5;
			c.rect.h = dest.h / 1.5;

			c.offset.x = (dest.w  - c.rect.w) / 2.0f;
			c.offset.y = (dest.h - c.rect.h) / 2.0f;

			e.addComponent<ProjectileTag>();
		});


	// Add timeline object (for testing danmaku scripting).
	auto& timelineManager(world.createEntity());
	auto& debugTimeline = timelineManager.addComponent<Timeline>();

	debugTimeline.timeline.emplace_back(1.0, [&radialSpawner] {
		std::cout << "Radial start!" << std::endl;
		radialSpawner.isActive = true;
	});
	debugTimeline.timeline.emplace_back(2.0, [&linearSpawner] {
		std::cout << "Linear start!" << std::endl;
		linearSpawner.isActive = true;
	});
	debugTimeline.timeline.emplace_back(10.0, [&radialSpawner] {
		std::cout << "Radial end!" << std::endl;
		radialSpawner.isActive = false;
	});
	debugTimeline.timeline.emplace_back(11.0, [&linearSpawner] {
		std::cout << "Linear end!" << std::endl;
		linearSpawner.isActive = false;
	});

	// Add scene state.
	auto& state(world.createEntity());
	state.addComponent<SceneState>();

	// Create pause menu overlay.
	auto& pauseMenuOverlay = UIUtils::createStageOverlay(world, windowWidth, windowHeight, "../asset/ui/darken.png",
		[this](Entity& overlay, int w, int h) {
			createPauseMenuUComponents(overlay, w, h);
		},
		[this](Entity& overlay) {
			bool isOpen = UIUtils::toggleOverlayVisibility(overlay);
			world.getEventManager().emit(PauseEvent{isOpen});

			// Play sound effect when opening pause menu.
			if (!isOpen) {
				AudioManager::playSfx("pause");
			}
		}
	);
	pauseMenuOverlay.addComponent<PauseMenuTag>();

	// Create continue game overlay.
	auto& continueGameOverlay = UIUtils::createStageOverlay(world, windowWidth, windowHeight, "../asset/ui/darken.png",
		[this](Entity& overlay, int w, int h) {
			createContinueGameUIComponents(overlay, w, h);
		},
		[this](Entity& overlay) {
			bool isOpen = UIUtils::toggleOverlayVisibility(overlay);

			// Disable pause menu while continue game menu is open.
			for (auto& entity : world.getEntities()) {
				if (entity->hasComponent<PauseMenuTag>() && entity->hasComponent<Toggleable>()) {
					entity->getComponent<Toggleable>().enabled = !isOpen;
					world.getEventManager().emit(PauseEvent{isOpen});

					break;
				}
			}

			// Play sound effect when opening continue menu.
			if (!isOpen) {
				AudioManager::playSfx("pause");
			}
		}
	);
	continueGameOverlay.addComponent<ContinueGameMenuTag>();

	// Create FPS counter label.
	auto& fpsCounter = UIUtils::createLabel(world, windowWidth - 170, windowHeight - 40, {240, 240, 240, 255},
		"pop1", "0.000fps", "fpsCounter", LabelType::FPSCounter);
	fpsCounter.addComponent<FPSCounter>();

	// Create sidebar UI labels.
	createSidebarUILabels(windowWidth, windowHeight, stageWidth, stageHeight);
}

void Scene::createSidebarUILabels(int windowWidth, int windowHeight, float stageWidth, float stageHeight) {
	const char* staticLabelFont = "DFPPOPCorn";
	const char* dynamicLabelFont = "pop1";

	// The base distance the UI labels should be from the left and top of the screen.
	int paddingX = static_cast<int>(windowWidth * 0.05);
	int paddingY = static_cast<int>((static_cast<float>(windowHeight) - stageHeight) / 2 * 3);

	// Font height and leading for calculating how distance is needed between each row of labels.
	float fontHeight = TTF_GetFontSize(AssetManager::getFont(staticLabelFont));
	float leading = 5.0f;

	// The distance for each column from the left side of the screen.
	// The static labels are the unchanging text labels (i.e. HiScore:) while the dynamic labels are the actual number value.
	int staticLeftPadding = static_cast<int>(stageWidth + static_cast<float>(paddingX) + 35);
	int dynamicLeftPadding = staticLeftPadding + 142;

	// Colours for the labels.
	SDL_Color offWhite = {240, 240, 240, 255};
	SDL_Color grey = {171, 166, 169, 255};
	SDL_Color lightPink = {170, 126, 176, 255};
	SDL_Color hotPink = {180, 85, 172, 255};

	// HiScore static and dynamic labels.
	UIUtils::createLabel(world, staticLeftPadding, paddingY, grey, staticLabelFont,
		"HiScore", "HiScoreLabel", LabelType::Static);
	UIUtils::createLabel(world, dynamicLeftPadding, paddingY, offWhite, dynamicLabelFont,
		"000000000", "HiScore", LabelType::HiScore);

	// Score static and dynamic labels.
	UIUtils::createLabel(world, staticLeftPadding, (fontHeight + leading) + paddingY, grey, staticLabelFont,
		"Score","ScoreLabel", LabelType::Static);
	UIUtils::createLabel(world, dynamicLeftPadding, (fontHeight + leading) + paddingY, offWhite, dynamicLabelFont,
		"000000000","Score", LabelType::Score);

	// Player health static and dynamic labels.
	UIUtils::createLabel(world, staticLeftPadding, (fontHeight + leading) * 2 + paddingY * 1.5, lightPink, staticLabelFont,
		"Player", "HealthLabel", LabelType::Static);
	UIUtils::createIconLabel(world, dynamicLeftPadding, (fontHeight + leading) * 2 + paddingY * 1.5, PlayerStats::MAX_HEALTH,
		fontHeight, fontHeight, IconCounterType::Health, "../asset/ui/red-star.png");

	// Bomb static and dynamic labels.
	UIUtils::createLabel(world, staticLeftPadding, (fontHeight + leading) * 3 + paddingY * 1.5, lightPink, staticLabelFont,
		"Bomb", "BombLabel", LabelType::Static);
	UIUtils::createIconLabel(world, dynamicLeftPadding, (fontHeight + leading) * 3 + paddingY * 1.5, PlayerStats::MAX_BOMBS,
		fontHeight, fontHeight, IconCounterType::Bomb, "../asset/ui/blue-star.png");

	// Power static and dynamic labels.
	UIUtils::createLabel(world, staticLeftPadding, (fontHeight + leading) * 4 + paddingY * 2, hotPink, staticLabelFont,
		"Power", "PowerLabel", LabelType::Static);
	UIUtils::createLabel(world, dynamicLeftPadding, (fontHeight + leading) * 4 + paddingY * 2, offWhite, dynamicLabelFont,
		"0", "Power", LabelType::Power);

	// Graze static and dynamic labels. Currently hidden since not implemented.
	// UIUtils::createLabel(world, staticLeftPadding, (fontHeight + leading) * 5 + paddingY * 2, hotPink, staticLabelFont,
	// 	"Graze", "GrazeLabel", LabelType::Static);
	// UIUtils::createLabel(world, dynamicLeftPadding, (fontHeight + leading) * 5 + paddingY * 2, offWhite, dynamicLabelFont,
	// 	"0", "Graze", LabelType::Graze);

	// Point static and dynamic labels.
	std::string initialPointString = "0/" + std::to_string(PlayerStats::MAX_POINTS);
	const char* initialPoint = initialPointString.c_str();

	UIUtils::createLabel(world, staticLeftPadding, (fontHeight + leading) * 5 + paddingY * 2, hotPink, staticLabelFont,
		"Point", "PointLabel", LabelType::Static);
	UIUtils::createLabel(world, dynamicLeftPadding, (fontHeight + leading) * 5 + paddingY * 2, offWhite, dynamicLabelFont,
		initialPoint, "Point", LabelType::Point);
}

void Scene::createPauseMenuUComponents(Entity& overlay, int windowWidth, int windowHeight) {
	if (!overlay.hasComponent<Toggleable>()) {
		std::cerr << "Overlay must have Toggleable component!" << std::endl;
	}

	// Label colours and font.
	SDL_Color selectedColour {202, 101, 101, 255};
	SDL_Color unselectedColour {48, 55, 69, 255};
	const char* font = "pop1";

	// Create resume button.
	auto& resumeButton =  UIUtils::createSelectableButton(world, font, selectedColour, unselectedColour,
		"Resume Game", "ResumeButton", [] {
			// Resume game by pushing an escape key down event to toggle the pause menu.
			SDL_Event event;
			event.type = SDL_EVENT_KEY_DOWN;
			event.key.key = SDLK_ESCAPE;
			SDL_PushEvent(&event);
		});

	// Create quit to title button.
	auto& quitTitleButton =  UIUtils::createSelectableButton(world, font, selectedColour, unselectedColour,
		"Quit To Title", "PauseQuitTitleButton", [] {
			// Request scene change to main menu.
			Game::onSceneChangeRequest("mainmenu");
		});

	// Create quit game button.
	auto& quitButton =  UIUtils::createSelectableButton(world, font, selectedColour, unselectedColour,
		"Quit Game", "QuitButton", [] {
			// Quit game by pushing a quit event.
			SDL_Event event;
			event.type = SDL_EVENT_QUIT;
			SDL_PushEvent(&event);
		});

	std::vector<Entity*> buttons;
	buttons.push_back(&resumeButton);
	buttons.push_back(&quitTitleButton);
	buttons.push_back(&quitButton);

	// Create the overlay with title and buttons.
	createOverlayUIComponents(overlay, windowWidth, windowHeight, "Pause", "PauseLabel", buttons);
}

void Scene::createContinueGameUIComponents(Entity& overlay, int windowWidth, int windowHeight) {
	if (!overlay.hasComponent<Toggleable>()) {
		std::cerr << "Overlay must have Toggleable component!" << std::endl;
	}

	// Label colours and font.
	SDL_Color selectedColour {202, 101, 101, 255};
	SDL_Color unselectedColour {48, 55, 69, 255};
	const char* font = "pop1";

	// Create continue game button.
	auto& toggleComponent = overlay.getComponent<Toggleable>();
	auto& continueButton =  UIUtils::createSelectableButton(world, font, selectedColour, unselectedColour,
		"Continue Game", "ContinueButton", [this, toggleComponent] {
			for (auto& entity : world.getEntities()) {
				if (entity->hasComponent<PlayerStats>()) {
					auto& playerStats = entity->getComponent<PlayerStats>();

					// Set player health back to 3 and reset score to 0.
					playerStats.currentHealth = 3;
					playerStats.currentScore = 0;

					break;
				}
			}

			// Toggle overlay.
			toggleComponent.toggle();
		});

	// Create quit to title button.
	auto& quitButton =  UIUtils::createSelectableButton(world, font, selectedColour, unselectedColour,
		"Quit To Title", "ContinueQuitTitleButton", [] {
			// Request scene change to main menu.
			Game::onSceneChangeRequest("mainmenu");
		});

	std::vector<Entity*> buttons;
	buttons.push_back(&continueButton);
	buttons.push_back(&quitButton);

	// Create the overlay with title and buttons.
	createOverlayUIComponents(overlay, windowWidth, windowHeight, "Continue? Score will be reset", "ContinueLabel", buttons);
}

void Scene::createOverlayUIComponents(Entity& overlay, int windowWidth, int windowHeight, const char* titleText,
	const char* titleCacheKey, const std::vector<Entity*> &selectableButtons) {
	if (!overlay.hasComponent<Children>()) {
		overlay.addComponent<Children>();
	}

	auto& parentChildren = overlay.getComponent<Children>();

	// Label colours and font.
	SDL_Color titleColour {240, 240, 240, 255};
	const char* font = "pop1";
	float fontHeight = TTF_GetFontSize(AssetManager::getFont(font));

	auto& overlaySprite = overlay.getComponent<Sprite>();

	float overlayMiddleX = overlaySprite.dst.w / 2;
	float overlayMiddleY = overlaySprite.dst.h / 2;
	float stagePaddingX = StageUtils::CalculateStagePaddingX(windowWidth);
	float stagePaddingY = StageUtils::CalculateStagePaddingY(windowHeight);

	// Create title label.
	auto& continueTitle =  UIUtils::createLabel(world, 0, 0, titleColour, font, titleText, titleCacheKey, LabelType::Static);
	auto& pauseTransform = continueTitle.getComponent<Transform>();
	auto& pauseLabel = continueTitle.getComponent<Label>();
	pauseLabel.visible = false;

	// Position label centered horizontally and slightly up the screen vertically.
	pauseTransform.position.x = stagePaddingX + overlayMiddleX - pauseLabel.texture->w / 2.0f;
	pauseTransform.position.y = stagePaddingY + overlayMiddleY - pauseLabel.texture->h / 2.0f - fontHeight * 3;

	// Add overlay as parent to the label.
	continueTitle.addComponent<Parent>(&overlay);
	parentChildren.children.push_back(&continueTitle);

	// Set up all the selectable buttons.
	Entity* firstEntity = nullptr;
	SelectableUI* firstSelectable = nullptr;

	Entity* previousEntity = nullptr;
	SelectableUI* previousSelectable = nullptr;

	float buttonIndex = 0.0f;
	for (auto buttonEntity : selectableButtons) {
		auto& buttonTransform = buttonEntity->getComponent<Transform>();
		auto& buttonLabel = buttonEntity->getComponent<Label>();

		// Add overlay as parent to the button.
		buttonEntity->addComponent<Parent>(&overlay);
		parentChildren.children.push_back(buttonEntity);

		// Position button
		buttonTransform.position.x = stagePaddingX + overlayMiddleX - buttonLabel.texture->w / 2.0f;
		buttonTransform.position.y = stagePaddingY + overlayMiddleY - buttonLabel.texture->h / 2.0f + buttonIndex * (fontHeight + 20);
		auto& buttonSelectable = buttonEntity->getComponent<SelectableUI>();

		// Set up doubly linked list for selectable UI.
		if (previousEntity != nullptr && previousSelectable != nullptr) {
			buttonSelectable.previous = previousEntity;
			previousSelectable->next = buttonEntity;
		} else {
			firstEntity = buttonEntity;
			firstSelectable = &buttonSelectable;
		}

		previousEntity = buttonEntity;
		previousSelectable = &buttonSelectable;

		buttonIndex++;
	}

	// Set up the first and list selectables to wrap around.
	if (firstSelectable != nullptr && previousSelectable != nullptr) {
		firstSelectable->previous = previousEntity;
		previousSelectable->next = firstEntity;
	}
}