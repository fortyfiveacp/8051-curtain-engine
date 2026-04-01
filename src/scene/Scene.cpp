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

		const auto& pauseEvent = static_cast<const PauseEvent&>(e);
		isPaused = pauseEvent.isPaused;
	});

	// Subscribe to event for debugging the game.
	world.getEventManager().subscribe([this](const BaseEvent& e) {
		if (e.type != EventType::Debug) {
			return;
		}

		const auto& debugEvent = static_cast<const DebugEvent&>(e);
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
		Vector2D(playerStartingX, playerStartingY)); // TODO: remove test values.

	// TODO: purge.
	auto& spawner(world.createEntity());
	Transform t = spawner.addComponent<Transform>(Vector2D(stageWidth - 150, stageHeight - 5), 0.0f, 1.0f);
	spawner.addComponent<TimedSpawner>(2.0f, [this, t] {
		// Create the projectile (birds).
		auto& e(world.createDeferredEntity());
		e.addComponent<Transform>(Vector2D(t.position.x, t.position.y), 0.0f, 1.0f);
		e.addComponent<Velocity>(Vector2D(0, -1), 100.0f);

		Animation anim = AssetManager::getAnimation("enemy");
		e.addComponent<Animation>(anim);

		SDL_Texture* tex = TextureManager::load("../asset/animations/bird_anim.png");
		SDL_FRect src { 0, 0, 32, 32 };
		SDL_FRect dest { t.position.x, t.position.y, 32, 32 };
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

			SDL_Texture* tex = TextureManager::load("../asset/animations/bird_anim.png");
			SDL_FRect src { 0, 0, 32, 32 };
			SDL_FRect dest { radialDanmakuTransform.position.x, radialDanmakuTransform.position.y, 32, 32 };
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

			SDL_Texture* tex = TextureManager::load("../asset/animations/bird_anim.png");
			SDL_FRect src { 0, 0, 32, 32 };
			SDL_FRect dest { position.x, position.y, 32, 32 };
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
	createPauseMenuOverlay(windowWidth, windowHeight);

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
	int paddingX = windowWidth * 0.05;
	int paddingY = (windowHeight - stageHeight) / 2 * 3;

	// Font height and leading for calculating how distance is needed between each row of labels.
	int fontHeight = TTF_GetFontSize(AssetManager::getFont(staticLabelFont));
	int leading = 5;

	// The distance for each column from the left side of the screen.
	// The static labels are the unchanging text labels (i.e. HiScore:) while the dynamic labels are the actual number value.
	int staticLeftPadding = stageWidth + paddingX + 35;
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

	// Graze static and dynamic labels.
	UIUtils::createLabel(world, staticLeftPadding, (fontHeight + leading) * 5 + paddingY * 2, hotPink, staticLabelFont,
		"Graze", "GrazeLabel", LabelType::Static);
	UIUtils::createLabel(world, dynamicLeftPadding, (fontHeight + leading) * 5 + paddingY * 2, offWhite, dynamicLabelFont,
		"0", "Graze", LabelType::Graze);

	// Point static and dynamic labels.
	std::string initialPointString = "0/" + std::to_string(PlayerStats::MAX_POINTS);
	const char* initialPoint = initialPointString.c_str();

	UIUtils::createLabel(world, staticLeftPadding, (fontHeight + leading) * 6 + paddingY * 2, hotPink, staticLabelFont,
		"Point", "PointLabel", LabelType::Static);
	UIUtils::createLabel(world, dynamicLeftPadding, (fontHeight + leading) * 6 + paddingY * 2, offWhite, dynamicLabelFont,
		initialPoint, "Point", LabelType::Point);
}

Entity& Scene::createPauseMenuOverlay(int windowWidth, int windowHeight) {
	auto &overlay(world.createEntity());
	SDL_Texture *overlayTex = TextureManager::load("../asset/ui/darken.png");
	SDL_FRect overlaySrc {0, 0, static_cast<float>(overlayTex->w), static_cast<float>(overlayTex->h)};
	SDL_FRect overlayDest = StageUtils::CalculateStageRect(windowWidth, windowHeight);
	overlay.addComponent<Transform>(Vector2D(overlayDest.x, overlayDest.y), 0.0f, 1.0f);
	overlay.addComponent<Sprite>(overlayTex, overlaySrc, overlayDest, RenderLayer::UI, false);

	createPauseMenuUComponents(overlay, windowWidth, windowHeight);

	// Add a toggleable component so the escape key can toggle the pause menu.
	overlay.addComponent<Toggleable>([this, &overlay]() {
		toggleOverlayVisibility(overlay);
	});

	overlay.addComponent<PauseMenuTag>();

	return overlay;
}

void Scene::createPauseMenuUComponents(Entity& overlay, int windowWidth, int windowHeight) {
	if (!overlay.hasComponent<Children>()) {
		overlay.addComponent<Children>();
	}

	auto& parentChildren = overlay.getComponent<Children>();

	// Label colours and font.
	SDL_Color selectedColour {202, 101, 101, 255};
	SDL_Color unselectedColour {48, 55, 69, 255};
	SDL_Color pausedColour {240, 240, 240, 255};
	const char* font = "pop1";
	float fontHeight = TTF_GetFontSize(AssetManager::getFont(font));

	auto& overlaySprite = overlay.getComponent<Sprite>();

	float overlayMiddleX = overlaySprite.dst.w / 2;
	float overlayMiddleY = overlaySprite.dst.h / 2;
	float stagePaddingX = StageUtils::CalculateStagePaddingX(windowWidth);
	float stagePaddingY = StageUtils::CalculateStagePaddingY(windowHeight);

	// Create paused title label.
	auto& pauseTitle =  UIUtils::createLabel(world, 0, 0, pausedColour, font,
		"Pause", "PauseLabel", LabelType::Static);
	auto& pauseTransform = pauseTitle.getComponent<Transform>();
	auto& pauseLabel = pauseTitle.getComponent<Label>();
	pauseLabel.visible = false;

	// Have to position the label after it's texture has been created.
	pauseTransform.position.x = stagePaddingX + overlayMiddleX - pauseLabel.texture->w / 2;
	pauseTransform.position.y = stagePaddingY + overlayMiddleY - pauseLabel.texture->h / 2 - fontHeight * 3;

	// Add overlay as parent to the label.
	pauseTitle.addComponent<Parent>(&overlay);
	parentChildren.children.push_back(&pauseTitle);

	// Create resume button.
	auto& resumeButton =  UIUtils::createSelectableButton(world, overlay, font, selectedColour, unselectedColour,
		"Resume Game", "ResumeButton", [] {
			// Resume game by pushing an escape key down event to toggle the pause menu.
			SDL_Event event;
			event.type = SDL_EVENT_KEY_DOWN;
			event.key.key = SDLK_ESCAPE;
			SDL_PushEvent(&event);
		});
	auto& resumeTransform = resumeButton.getComponent<Transform>();
	auto& resumeLabel = resumeButton.getComponent<Label>();

	resumeTransform.position.x = stagePaddingX + overlayMiddleX - resumeLabel.texture->w / 2;
	resumeTransform.position.y = stagePaddingY + overlayMiddleY - resumeLabel.texture->h / 2;
	auto& resumeSelectable = resumeButton.getComponent<SelectableUI>();

	// Create quit button.
	auto& quitButton =  UIUtils::createSelectableButton(world, overlay, font, selectedColour, unselectedColour,
		"Quit Game", "QuitButton", [] {
			// Quit game by pushing a quit event.
			SDL_Event event;
			event.type = SDL_EVENT_QUIT;
			SDL_PushEvent(&event);
		});
	auto& quitTransform = quitButton.getComponent<Transform>();
	auto& quitLabel = quitButton.getComponent<Label>();

	quitTransform.position.x = stagePaddingX + overlayMiddleX - quitLabel.texture->w / 2;
	quitTransform.position.y = stagePaddingY + overlayMiddleY - quitLabel.texture->h / 2 + fontHeight + 15;
	auto& quitSelectable = quitButton.getComponent<SelectableUI>();

	// Set up selection doubly linked list.
	resumeSelectable.next = &quitButton;
	resumeSelectable.previous = &quitButton;

	quitSelectable.next = &resumeButton;
	quitSelectable.previous = &resumeButton;
}

void Scene::toggleOverlayVisibility(Entity& overlay) {
	auto& sprite = overlay.getComponent<Sprite>();
	bool newVisibility = !sprite.visible;
	sprite.visible = newVisibility;

	if (newVisibility) {
		AudioManager::playSfx("pause");
	}

	if (overlay.hasComponent<Children>()) {
		auto& c = overlay.getComponent<Children>();

		for (auto& child : c.children) {
			if (child && child->hasComponent<Label>()) {
				child->getComponent<Label>().visible = newVisibility;
			}

			if (child && child->hasComponent<SelectableUI>()) {
				// Make sure all entities aren't selected.
				auto& selectable = child->getComponent<SelectableUI>();
				selectable.selected = false;
				selectable.onReleased();
			}
		}

		if (newVisibility) {
			// Set the first selectable entity as the default selected, if visible.
			for (auto& child : c.children) {
				if (child && child->hasComponent<SelectableUI>()) {
					auto& selected = child->getComponent<SelectableUI>();
					selected.selected = true;
					selected.onSelect();
					break;
				}
			}
		}
	}
}