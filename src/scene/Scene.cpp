#include "../manager/AssetManager.h"
#include "Game.h"
#include "Scene.h"

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

	auto& fpsCounter = createLabel(windowWidth - 170, windowHeight - 40, {240, 240, 240, 255},
		"pop1", "0.000fps", "fpsCounter", LabelType::FPSCounter);
	fpsCounter.addComponent<FPSCounter>();
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
	float stageWidth = StageUtils::CalculateStageWidth(windowWidth);
	float stageHeight = StageUtils::CalculateStageHeight(windowHeight);
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

	// Create the player.
	auto& player (world.createEntity());
	player.addComponent<Velocity>(Vector2D(0.0f, 0.0f), 380.0f);

	Animation anim = AssetManager::getAnimation("player");
	player.addComponent<Animation>(anim);

	SDL_Texture* texture = TextureManager::load("../asset/animations/reimu_anim.png");
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
	player.addComponent<PlayerStats>(Game::gameState.playerHealth, Game::gameState.playerBombs, 1234, 5678, 9, 10, 11); // TODO: remove test values.

	// TODO: purge.
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

		Collider& c = e.addComponent<Collider>("projectile");
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

	// Create pause menu overlay.
	createPauseMenuOverlay(windowWidth, windowHeight);

	// Create FPS counter label.
	auto& fpsCounter = createLabel(windowWidth - 170, windowHeight - 40, {240, 240, 240, 255},
		"pop1", "0.000fps", "fpsCounter", LabelType::FPSCounter);
	fpsCounter.addComponent<FPSCounter>();

	// Create sidebar UI labels.
	createSidebarUILabels(windowWidth, windowHeight, stageWidth, stageHeight);
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
	int fontHeight = TTF_GetFontSize(AssetManager::getFont(font));

	auto& overlaySprite = overlay.getComponent<Sprite>();

	int overlayMiddleX = overlaySprite.dst.w / 2;
	int overlayMiddleY = overlaySprite.dst.h / 2;
	int stagePaddingX = StageUtils::CalculateStagePaddingX(windowWidth);
	int stagePaddingY = StageUtils::CalculateStagePaddingY(windowHeight);

	// Create paused title label.
	auto& pauseTitle = createLabel(0, 0, pausedColour, font,
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
	auto& resumeButton = createSelectableButton(overlay, font, selectedColour, unselectedColour,
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
	auto& quitButton = createSelectableButton(overlay, font, selectedColour, unselectedColour,
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
	resumeSelectable.next = &quitSelectable;
	resumeSelectable.previous = &quitSelectable;

	quitSelectable.next = &resumeSelectable;
	quitSelectable.previous = &resumeSelectable;
}

Entity& Scene::createSelectableButton(Entity& overlay, const char* font, SDL_Color selectedColour, SDL_Color unselectedColour,
	const char* text, const char* cacheKey, std::function<void()> onPressed) {
	auto& parentChildren = overlay.getComponent<Children>();

	// Create the button.
	auto& button = createLabel(0, 0, selectedColour, font,
		text, cacheKey, LabelType::Static);
	auto& transform = button.getComponent<Transform>();
	auto& label = button.getComponent<Label>();
	label.visible = false;

	auto& selectable = button.addComponent<SelectableUI>();

	// Functions for on press, on release and on select.
	selectable.onPressed = [&transform, onPressed] {
		AudioManager::playSfx("ok");
		transform.scale = 1.0f;
		onPressed();
	};

	selectable.onReleased = [&transform, &label, unselectedColour] {
		transform.scale = 1.0f;
		label.color = unselectedColour;
		label.dirty = true;
	};

	selectable.onSelect = [&transform, &label, selectedColour] {
		AudioManager::playSfx("select");
		transform.scale = 1.15f;
		label.color = selectedColour;
		label.dirty = true;
	};

	// Add overlay as parent to the button.
	button.addComponent<Parent>(&overlay);
	parentChildren.children.push_back(&button);

	return button;
}

void Scene::toggleOverlayVisibility(Entity& overlay) {
	AudioManager::playSfx("pause");

	auto& sprite = overlay.getComponent<Sprite>();
	bool newVisibility = !sprite.visible;
	sprite.visible = newVisibility;

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

Entity& Scene::createLabel(int x, int y, SDL_Color colour, const char* fontName, const char* text, const char* cacheKey,
	LabelType labelType) {
	auto& newLabel(world.createEntity());
	Label label = {
		text,
		AssetManager::getFont(fontName),
		colour,
		labelType,
		cacheKey
	};

	TextureManager::loadLabel(label);
	newLabel.addComponent<Label>(label);
	newLabel.addComponent<Transform>(Vector2D(x, y), 0.0f, 1.0f);

	return newLabel;
}

Entity& Scene::createIconLabel(int x, int y, int maxNumber, float iconWidth, float iconHeight,
	IconCounterType iconCounterType, const char* texturePath) {

	SDL_Texture* tex = TextureManager::load(texturePath);

	auto& iconLabel(world.createEntity());
	iconLabel.addComponent<IconCounter>(maxNumber, tex, iconCounterType);
	iconLabel.addComponent<Transform>(Vector2D(x, y), 0.0f, 1.0f);

	iconLabel.addComponent<Children>();

	for (int i = 0; i < maxNumber; i++) {
		auto& icon(world.createEntity());
		SDL_FRect src {0, 0, static_cast<float>(tex->w), static_cast<float>(tex->h)};
		SDL_FRect dst = {0, 0, iconWidth, iconHeight};
		icon.addComponent<Sprite>(tex, src, dst, RenderLayer::UI);
		icon.addComponent<Transform>(Vector2D(x + iconWidth * i, y), 0.0f, 1.0f);

		// Parent icon to the icon label.
		icon.addComponent<Parent>(&iconLabel);
		iconLabel.getComponent<Children>().children.push_back(&icon);
	}

	return iconLabel;
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
	createLabel(staticLeftPadding, paddingY, grey, staticLabelFont,
		"HiScore", "HiScoreLabel", LabelType::Static);
	createLabel(dynamicLeftPadding, paddingY, offWhite, dynamicLabelFont,
		"000000000", "HiScore", LabelType::HiScore);

	// Score static and dynamic labels.
	createLabel(staticLeftPadding, (fontHeight + leading) + paddingY, grey, staticLabelFont,
		"Score","ScoreLabel", LabelType::Static);
	createLabel(dynamicLeftPadding, (fontHeight + leading) + paddingY, offWhite, dynamicLabelFont,
		"000000000","Score", LabelType::Score);

	// Player health static and dynamic labels.
	createLabel(staticLeftPadding, (fontHeight + leading) * 2 + paddingY * 1.5, lightPink, staticLabelFont,
		"Player", "HealthLabel", LabelType::Static);
	createIconLabel(dynamicLeftPadding, (fontHeight + leading) * 2 + paddingY * 1.5, 8,
		fontHeight, fontHeight, IconCounterType::Health, "../asset/ui/red-star.png");

	// Bomb static and dynamic labels.
	createLabel(staticLeftPadding, (fontHeight + leading) * 3 + paddingY * 1.5, lightPink, staticLabelFont,
		"Bomb", "BombLabel", LabelType::Static);
	createIconLabel(dynamicLeftPadding, (fontHeight + leading) * 3 + paddingY * 1.5, 8,
		fontHeight, fontHeight, IconCounterType::Bomb, "../asset/ui/blue-star.png");

	// Power static and dynamic labels.
	createLabel(staticLeftPadding, (fontHeight + leading) * 4 + paddingY * 2, hotPink, staticLabelFont,
		"Power", "PowerLabel", LabelType::Static);
	createLabel(dynamicLeftPadding, (fontHeight + leading) * 4 + paddingY * 2, offWhite, dynamicLabelFont,
		"0", "Power", LabelType::Power);

	// Graze static and dynamic labels.
	createLabel(staticLeftPadding, (fontHeight + leading) * 5 + paddingY * 2, hotPink, staticLabelFont,
		"Graze", "GrazeLabel", LabelType::Static);
	createLabel(dynamicLeftPadding, (fontHeight + leading) * 5 + paddingY * 2, offWhite, dynamicLabelFont,
		"0", "Graze", LabelType::Graze);

	// Point static and dynamic labels.
	createLabel(staticLeftPadding, (fontHeight + leading) * 6 + paddingY * 2, hotPink, staticLabelFont,
		"Point", "PointLabel", LabelType::Static);
	createLabel(dynamicLeftPadding, (fontHeight + leading) * 6 + paddingY * 2, offWhite, dynamicLabelFont,
		"0/0", "Point", LabelType::Point);
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
	stageBackground.addComponent<StageBackground>(scrollSpeedY, 0.0f, tex);

	return stageBackground;
}