#include "../manager/AssetManager.h"
#include "Game.h"
#include "Scene.h"

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

	createFPSCounterLabel(windowWidth, windowHeight);
}

void Scene::initGameplay(const char* mapPath, int windowWidth, int windowHeight) {
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
	float stageWidth = windowWidth * 0.6;
	float stageHeight = windowHeight * 0.93;
	float backgroundSpeed = 60.0f;
	float foregroundSpeed = backgroundSpeed + backgroundSpeed * 0.25f;

	// Create backgrounds.
	// The backgrounds are 1 pixel taller to make an overlap that hides the seam between backgrounds.
	createStageBackground(stageWidth, stageHeight + 1, 0, backgroundSpeed, "../asset/stage1.png");
	createStageBackground(stageWidth, stageHeight + 1, -stageHeight, backgroundSpeed, "../asset/stage1.png");

	// Create foregrounds.
	createStageBackground(stageWidth, stageHeight, 0, foregroundSpeed, "../asset/foreground1.png");
	createStageBackground(stageWidth, stageHeight, -stageHeight, foregroundSpeed, "../asset/foreground1.png");

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
	SDL_FRect camView{};
	camView.w = windowWidth; // width of the window.
	camView.h = windowHeight; // height of the window.
	cam.addComponent<Camera>(camView, world.getMap().width * 32.0f, world.getMap().height * 32.0f);

	auto& player (world.createEntity());
	player.addComponent<Velocity>(Vector2D(0.0f, 0.0f), 380.0f);

	Animation anim = AssetManager::getAnimation("player");
	player.addComponent<Animation>(anim);

	SDL_Texture* texture = TextureManager::load("../asset/animations/reimu_anim.png");
	// SDL_FRect playerSrc {0, 0, 32, 44}; // for Mario.
	SDL_FRect playerSrc = anim.clips[anim.currentClip].frameIndices[0];

	float scaledPlayerWidth = playerSrc.w * 1.75f;
	float scaledPlayerHeight = playerSrc.h * 1.75f;
	auto& playerTransform = player.addComponent<Transform>(Vector2D(stageWidth / 2 - scaledPlayerWidth / 2, stageHeight - scaledPlayerHeight), 0.0f, 1.0f);
	SDL_FRect playerDst {playerTransform.position.x, playerTransform.position.y, scaledPlayerWidth, scaledPlayerHeight};

	player.addComponent<Sprite>(texture, playerSrc, playerDst);

	auto& playerCollider = player.addComponent<Collider>("player");
	playerCollider.rect.w = playerDst.w;
	playerCollider.rect.h = playerDst.h;

	player.addComponent<PlayerTag>();
	player.addComponent<Health>(Game::gameState.playerHealth);

	auto& spawner(world.createEntity());
	Transform t = spawner.addComponent<Transform>(Vector2D(windowWidth / 2, windowHeight - 5), 0.0f, 1.0f);
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

		Collider c = e.addComponent<Collider>("projectile");
		c.rect.w = dest.w;
		c.rect.h = dest.h;

		e.addComponent<ProjectileTag>();
	});

	// Add timeline object (experimental, this will actually spawn enemy convoys later).
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

	// Add scene state.
	auto& state(world.createEntity());
	state.addComponent<SceneState>();

	// Pause menu overlay.
	createPauseMenuOverlay(windowWidth, windowHeight);

	// Create FPS counter label.
	createFPSCounterLabel(windowWidth, windowHeight);

	// Create UI labels.
	createUILabels(windowWidth, windowHeight, stageWidth, stageHeight);
}

Entity& Scene::createPauseMenuOverlay(int windowWidth, int windowHeight) {
	auto &overlay(world.createEntity());
	SDL_Texture *overlayTex = TextureManager::load("../asset/ui/settings.jpg");
	SDL_FRect overlaySrc {0, 0, windowWidth * 0.85f, windowHeight  * 0.85f};
	SDL_FRect overlayDest {(float)windowWidth / 2 - overlaySrc.w / 2, (float)windowHeight / 2 - overlaySrc.h / 2, overlaySrc.w, overlaySrc.h};
	overlay.addComponent<Transform>(Vector2D(overlayDest.x, overlayDest.y), 0.0f, 1.0f);
	overlay.addComponent<Sprite>(overlayTex, overlaySrc, overlayDest, RenderLayer::UI, false);

	createPauseMenuUComponents(overlay);

	// Toggleable component so escape key can toggle the pause menu.
	overlay.addComponent<Toggleable>([this, &overlay]() {
		toggleOverlayVisibility(overlay);
	});

	overlay.addComponent<PauseMenuTag>();

	return overlay;
}

void Scene::createPauseMenuUComponents(Entity& overlay) {
	if (!overlay.hasComponent<Children>()) {
		overlay.addComponent<Children>();
	}

	auto& overlayTransform = overlay.getComponent<Transform>();
	auto& overlaySprite = overlay.getComponent<Sprite>();

	float baseX = overlayTransform.position.x;
	float baseY = overlayTransform.position.y;

	auto& closeButton = world.createEntity();
	auto& closeTransform = closeButton.addComponent<Transform>(Vector2D(baseX + overlaySprite.dst.w - 40, baseY + 10), 0.0f, 1.0f);

	SDL_Texture* texture = TextureManager::load("../asset/ui/close.png");
	SDL_FRect closeSrc {0, 0, 32, 32};
	SDL_FRect closeDst {closeTransform.position.x, closeTransform.position.y, closeSrc.w, closeSrc.h};
	closeButton.addComponent<Sprite>(texture, closeSrc, closeDst, RenderLayer::UI, false);
	closeButton.addComponent<Collider>("ui", closeDst);

	auto& selectable = closeButton.addComponent<SelectableUI>();

	selectable.onPressed = [this, &overlay, &closeTransform] {
		toggleOverlayVisibility(overlay);
		std::cout << "button 1 pressed" << std::endl;
		closeTransform.scale = 1.0f;
	};

	selectable.onReleased = [&closeTransform] {
		closeTransform.scale = 1.0f;
	};

	selectable.onSelect = [&closeTransform] {
		closeTransform.scale = 1.25f;
	};

	closeButton.addComponent<Parent>(&overlay);
	auto& parentChildren = overlay.getComponent<Children>();
	parentChildren.children.push_back(&closeButton);

	// Second close button for testing.
	auto& closeButton2 = world.createEntity();
	auto& closeTransform2 = closeButton2.addComponent<Transform>(Vector2D(baseX + overlaySprite.dst.w - 40, baseY + 50), 0.0f, 1.0f);
	SDL_FRect closeSrc2 {0, 0, 32, 32};
	SDL_FRect closeDst2 {closeTransform2.position.x, closeTransform2.position.y, closeSrc2.w, closeSrc2.h};
	closeButton2.addComponent<Sprite>(texture, closeSrc2, closeDst2, RenderLayer::UI, false);
	closeButton2.addComponent<Collider>("ui", closeDst2);

	auto& selectable2 = closeButton2.addComponent<SelectableUI>();

	selectable2.onPressed = [this, &overlay, &closeTransform2] {
		toggleOverlayVisibility(overlay);
		std::cout << "button 2 pressed" << std::endl;
		closeTransform2.scale = 1.0f;
	};

	selectable2.onReleased = [&closeTransform2] {
		closeTransform2.scale = 1.0f;
	};

	selectable2.onSelect = [&closeTransform2] {
		closeTransform2.scale = 1.25f;
	};

	closeButton2.addComponent<Parent>(&overlay);
	parentChildren.children.push_back(&closeButton2);

	// Set up selection doubly linked list.
	selectable.next = &selectable2;
	selectable.previous = &selectable2;

	selectable2.next = &selectable;
	selectable2.previous = &selectable;
}

void Scene::toggleOverlayVisibility(Entity& overlay) {
	auto& sprite = overlay.getComponent<Sprite>();
	bool newVisibility = !sprite.visible;
	sprite.visible = newVisibility;

	if (overlay.hasComponent<Children>()) {
		auto& c = overlay.getComponent<Children>();

		for (auto& child : c.children) {
			if (child && child->hasComponent<Sprite>()) {
				child->getComponent<Sprite>().visible = newVisibility;
			}

			if (child && child->hasComponent<Collider>()) {
				child->getComponent<Collider>().enabled = newVisibility;
			}

			if (child && child->hasComponent<SelectableUI>()) {
				// Make sure all entities aren't selected.
				auto& selectable = child->getComponent<SelectableUI>();
				selectable.selected = false;
				selectable.onReleased();
			}
		}

		// Set the first selectable entity as the default selected, if visible.
		if (newVisibility) {
			auto& selected = c.children.front()->getComponent<SelectableUI>();
			selected.selected = true;
			selected.onSelect();
		}
	}
}

// TODO: purge.
Entity& Scene::createPlayerPosLabel() {
	auto& playerPosLabel(world.createEntity());
	Label label = {
		"Test String",
		AssetManager::getFont("pop1"),
		{240, 240, 240, 255},
		LabelType::PlayerPosition,
		"playerPos"
	};

	TextureManager::loadLabel(label);
	playerPosLabel.addComponent<Label>(label);
	playerPosLabel.addComponent<Transform>(Vector2D(10, 10), 0.0f, 1.0f);

	return playerPosLabel;
}

Entity& Scene::createFPSCounterLabel(int windowWidth, int windowHeight) {
	auto& fpsCounterLabel(world.createEntity());
	Label label = {
		"0.000fps",
		AssetManager::getFont("pop1"),
		{240, 240, 240, 255},
		LabelType::FPSCounter,
		"fpsCounter"
	};

	TextureManager::loadLabel(label);
	fpsCounterLabel.addComponent<Label>(label);
	fpsCounterLabel.addComponent<Transform>(Vector2D(windowWidth - 170, windowHeight - 40), 0.0f, 1.0f);
	fpsCounterLabel.addComponent<FPSCounter>();

	return fpsCounterLabel;
}

Entity& Scene::createStaticLabel(int x, int y, SDL_Color colour, const char* fontName, const char* text) {
	auto& fpsCounterLabel(world.createEntity());
	Label label = {
		text,
		AssetManager::getFont(fontName),
		colour,
		LabelType::Static,
		text
	};
	// Immediately mark the label as dirty so it renders.
	label.dirty = true;

	TextureManager::loadLabel(label);
	fpsCounterLabel.addComponent<Label>(label);
	fpsCounterLabel.addComponent<Transform>(Vector2D(x, y), 0.0f, 1.0f);

	return fpsCounterLabel;
}

void Scene::createUILabels(int windowWidth, int windowHeight, float stageWidth, float stageHeight) {
	char* staticLabelFont = "DFPPOPCorn";

	int paddingX = windowWidth * 0.05;
	int paddingY = (windowHeight - stageHeight) / 2 * 3;

	int fontHeight = TTF_GetFontSize(AssetManager::getFont(staticLabelFont));
	int leading = 5;
	int leftPadding = stageWidth + paddingX + 50;

	SDL_Color grey = {171, 166, 169, 255};
	SDL_Color lightPink = {170, 126, 176, 255};
	SDL_Color hotPink = {180, 85, 172, 255};

	createStaticLabel(leftPadding, paddingY, grey, staticLabelFont, "HiScore");
	createStaticLabel(leftPadding, fontHeight + paddingY, grey, staticLabelFont, "Score");

	createStaticLabel(leftPadding, (fontHeight + leading) * 2 + paddingY * 1.5, lightPink, staticLabelFont, "Player");
	createStaticLabel(leftPadding, (fontHeight + leading) * 3 + paddingY * 1.5, lightPink, staticLabelFont, "Bomb");

	createStaticLabel(leftPadding, (fontHeight + leading) * 4 + paddingY * 2, hotPink, staticLabelFont, "Power");
	createStaticLabel(leftPadding, (fontHeight + leading) * 5 + paddingY * 2, hotPink, staticLabelFont, "Graze");
	createStaticLabel(leftPadding, (fontHeight + leading) * 6 + paddingY * 2, hotPink, staticLabelFont, "Point");

	// TODO: dynamic labels.
}

Entity& Scene::createStageBackground(float stageWidth, float stageHeight, float startingY, float scrollSpeedY, const char* texturePath) {
	SDL_Texture* tex = TextureManager::load(texturePath);
	SDL_FRect src {0, 0, static_cast<float>(tex->w), static_cast<float>(tex->h)};
	SDL_FRect dst = {0, 0, stageWidth, stageHeight};

	// Create backgrounds.
	auto& stageBackground = world.createEntity();
	stageBackground.addComponent<Transform>(Vector2D(0, startingY), 0.0f, 1.0f);
	stageBackground.addComponent<Velocity>(Vector2D(0, 1), scrollSpeedY);
	stageBackground.addComponent<Sprite>(tex, src, dst, RenderLayer::World);
	stageBackground.addComponent<StageBackground>(stageWidth, stageHeight, scrollSpeedY, 0.0f,tex);

	return stageBackground;
}