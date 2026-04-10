#include "../manager/AssetManager.h"
#include "Game.h"
#include "Scene.h"

#include "DanmakuFactory.h"
#include "ItemFactory.h"
#include "StageUtils.h"
#include "../manager/AudioManager.h"
#include "EnemyFactory.h"
#include "PlayerShotFactory.h"
#include "StageLoader.h"

Scene::Scene(SceneType sceneType, const char* sceneName, const char* stageDataPath, const char* stageBackgroundPath,
	const char* foregroundPath, const int windowWidth, const int windowHeight) : name(sceneName), type(sceneType) {

	switch (sceneType) {
		case SceneType::MainMenu:
			initMainMenu(windowWidth, windowHeight);
			break;
		case SceneType::Gameplay:
			initGameplay(stageDataPath, stageBackgroundPath, foregroundPath, windowWidth, windowHeight);
			break;
		case SceneType::Credits:
			initCredits(windowWidth, windowHeight);
			break;
		default:
			break;
	}
}

void Scene::initMainMenu(int windowWidth, int windowHeight) {
	AudioManager::playMusic("menu-theme");

	// Ensure game state is reset when back on the main menu.
	resetGameState();

	// Camera.
	auto& cam = world.createEntity();
	cam.addComponent<Camera>();

	// Create the main menu.
	auto width = static_cast<float>(windowWidth);
	auto height = static_cast<float>(windowHeight);

	// Menu background.
	UIUtils::createBackground(world, width, height, "../asset/menu/main-menu-background.png");

	// Menu characters.
	UIUtils::createFadeInBackgroundLayer(world, width, height, "../asset/menu/main-menu-characters.png", 1.25f, 0.3f);

	// Menu text.
	UIUtils::createFadeInBackgroundLayer(world, width, height, "../asset/menu/main-menu-text.png", 1.5f, 1.7f);

	// FPS counter.
	auto& fpsCounter = UIUtils::createLabel(world, windowWidth - 170, windowHeight - 40,
		{240, 240, 240, 255}, "pop1", "0.000fps", "fpsCounter", LabelType::FPSCounter);
	fpsCounter.addComponent<FPSCounter>();
}

void Scene::initGameplay(const char* stageDataPath, const char* stageBackgroundPath, const char* foregroundPath, int windowWidth, int windowHeight) {
	AudioManager::playMusic("stage-theme");

	// Subscribe to event for pausing the game.
	world.getEventManager().subscribe([this](const BaseEvent& e) {
		if (e.type != EventType::Pause) {
			return;
		}

		const auto& pauseEvent = dynamic_cast<const PauseEvent&>(e);
		isPaused = pauseEvent.isPaused;

		if (pauseEvent.isPaused) {
			AudioManager::pauseMusic();
		} else {
			AudioManager::resumeMusic();
		}
	});

	// Subscribe to event for debugging the game.
	world.getEventManager().subscribe([this](const BaseEvent& e) {
		if (e.type != EventType::Debug) {
			return;
		}

		const auto& debugEvent = dynamic_cast<const DebugEvent&>(e);
		isDebugging = debugEvent.isDebugging;
	});

	SDL_Texture* backgroundTex = TextureManager::load("../asset/ui/background.png");
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
	StageUtils::createStageBackground(world, stageWidth, stageHeight + 1, 0, backgroundSpeed, stageBackgroundPath);
	StageUtils::createStageBackground(world, stageWidth, stageHeight + 1, -stageHeight, backgroundSpeed, stageBackgroundPath);

	// Create foregrounds.
	StageUtils::createStageBackground(world, stageWidth, stageHeight, 0, foregroundSpeed, foregroundPath);
	StageUtils::createStageBackground(world, stageWidth, stageHeight, -stageHeight, foregroundSpeed, foregroundPath);

	StageLoader::loadStage(stageDataPath, world);

	auto& cam = world.createEntity();
	SDL_FRect camView {0, 0, stageWidth, stageHeight};
	float outOfViewPadding = 100.0f;
	cam.addComponent<Camera>(camView, stageWidth, stageHeight, outOfViewPadding);

	// Create the player.
	auto& player (world.createEntity());
	player.addComponent<Velocity>(Vector2D(0.0f, 0.0f), 380.0f);

	// Player animation.
	Animation anim = AssetManager::getAnimation("player");
	player.addComponent<Animation>(anim);

	// Player size and positioning.
	SDL_Texture* texture = TextureManager::load("../asset/animations/reimu_anim.png");
	SDL_FRect playerSrc = anim.clips[anim.currentClip].frameIndices[0];

	float scaledPlayerWidth = playerSrc.w * 1.75f;
	float scaledPlayerHeight = playerSrc.h * 1.75f;
	float playerStartingX = stageWidth / 2;
	float playerStartingY = stageHeight - scaledPlayerHeight;
	auto& playerTransform = player.addComponent<Transform>(Vector2D(playerStartingX, playerStartingY), 0.0f, 1.0f);
	SDL_FRect playerDst {playerTransform.position.x, playerTransform.position.y, scaledPlayerWidth, scaledPlayerHeight};

	Vector2D playerPivotOffset = Vector2D(playerDst.w / 2.0f, playerDst.h / 2.0f);
	player.addComponent<Sprite>(texture, playerSrc, playerDst, RenderLayer::World, playerPivotOffset);

	// Player gameplay components.
	auto& playerCircleCollider = player.addComponent<CircleCollider>("player");
	playerCircleCollider.radius = 4;
	player.addComponent<DeathBombState>();
	player.addComponent<PlayerBombAbility>();

	player.addComponent<PlayerTag>();
	player.addComponent<KeyboardInput>();
	player.addComponent<InvincibilityFrames>();
	player.addComponent<PlayerRespawn>(Vector2D(playerStartingX, playerStartingY));
	auto& playerStats = player.addComponent<PlayerStats>(
		Game::gameState.hiScore,
		Game::gameState.score,
		Game::gameState.playerHealth,
		Game::gameState.playerBombs,
		Game::gameState.power,
		Game::gameState.graze,
		Game::gameState.point,
		Game::gameState.continues
		);

	// Attach player shooting components.
	PlayerShotFactory::buildPlayerDanmaku(player, world, { playerStartingX, playerStartingY }, playerStats);

	// Add scene state.
	auto& state(world.createEntity());
	state.addComponent<SceneState>();

	// Create boss health bar.
	createBossHealthBar(windowWidth, windowHeight);

	// Create boss tracker.
	float stagePaddingX = StageUtils::CalculateStagePaddingX(windowWidth);
	float stagePaddingY = StageUtils::CalculateStagePaddingY(windowHeight);

	auto& bossTrackerEntity = world.createEntity();
	bossTrackerEntity.addComponent<Transform>(Vector2D(0, windowHeight - stagePaddingY), 0.0f, 1.0f);
	SDL_Texture* tex = TextureManager::load("../asset/ui/enemy-tag.png");
	SDL_FRect src {0, 0, static_cast<float>(tex->w), static_cast<float>(tex->h)};
	SDL_FRect dst = {0, 0, static_cast<float>(tex->w) * 0.8f, stagePaddingY};
	auto& sprite = bossTrackerEntity.addComponent<Sprite>(tex, src, dst, RenderLayer::UI);
	sprite.visible = false;

	bossTrackerEntity.addComponent<BossTracker>(stagePaddingX, stageWidth + stagePaddingX);
	bossTrackerEntity.addComponent<Fade>(0.5f);

	// Create pause menu overlay.
	auto& pauseMenuOverlay = UIUtils::createStageOverlay(world, windowWidth, windowHeight, "../asset/ui/darken.png",
		[this](Entity& overlay, int w, int h) {
			createPauseMenuUComponents(overlay, w, h);
		},
		[this](Entity& overlay) {
			bool isOpen = UIUtils::toggleOverlayVisibility(overlay);
			world.getEventManager().emit(PauseEvent{isOpen});

			// Play sound effect when opening pause menu.
			if (isOpen) {
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

			// Disable the ability to toggle pause menu while continue game menu is open.
			for (auto& entity : world.getEntities()) {
				if (entity->hasComponent<PauseMenuTag>() && entity->hasComponent<Toggleable>()) {
					entity->getComponent<Toggleable>().enabled = !isOpen;
					world.getEventManager().emit(PauseEvent{isOpen});

					break;
				}
			}
		}
	);
	continueGameOverlay.addComponent<ContinueGameMenuTag>();

	// Create win game overlay.
	auto& winGameOverlay = UIUtils::createStageOverlay(world, windowWidth, windowHeight, "../asset/ui/win.png",
		[this](Entity& overlay, int w, int h) {
			createWinGameMenuUComponents(overlay, w, h);
		},
		[this](Entity& overlay) {
			bool isOpen = UIUtils::toggleOverlayVisibility(overlay);

			// Disable the ability to toggle pause menu while win game menu is open.
			for (auto& entity : world.getEntities()) {
				if (entity->hasComponent<PauseMenuTag>() && entity->hasComponent<Toggleable>()) {
					entity->getComponent<Toggleable>().enabled = !isOpen;
					world.getEventManager().emit(PauseEvent{isOpen});

					break;
				}
			}
		}
	);
	winGameOverlay.addComponent<WinGameMenuTag>();

	// Create FPS counter label.
	auto& fpsCounter = UIUtils::createLabel(world, windowWidth - 170, windowHeight - 40, {240, 240, 240, 255},
		"pop1", "0.000fps", "fpsCounter", LabelType::FPSCounter);
	fpsCounter.addComponent<FPSCounter>();

	// Create sidebar UI labels.
	createSidebarUILabels(windowWidth, windowHeight, stageWidth, stageHeight);
}

void Scene::initCredits(int windowWidth, int windowHeight) {
	AudioManager::playMusic("credits-theme");

	float width = static_cast<float>(windowWidth);
	float height = static_cast<float>(windowHeight);

	// Create credits background.
	auto& creditsBackground = UIUtils::createFadeInBackgroundLayer(world, width, height * 2,
		"../asset/credits/credits-background.png", 2.0f, 0.5f);
	auto& creditsTransform = creditsBackground.getComponent<Transform>();
	creditsTransform.position.y = -height;

	// Make the background slowly move downwards.
	creditsBackground.addComponent<Velocity>(Vector2D(0, 1), 7.0f);

	// Paths to all the credit textures, credits display in the order listed.
	std::vector<std::string> creditsPaths = {
		"../asset/credits/credits-title.png",
		"../asset/credits/credits-zun.png",
		"../asset/credits/credits-programmers.png",
		"../asset/credits/credits-sprites.png",
		"../asset/credits/credits-art.png",
		"../asset/credits/credits-sound.png",
		"../asset/credits/credits-special.png"
	};

	// Use a timeline to manage fading credits in/out and returning to main menu.
	auto& timelineEntity = world.createEntity();
	auto& creditsTimeline = timelineEntity.addComponent<Timeline>();
	createCreditsTimeline(creditsTimeline, creditsPaths, windowWidth, windowHeight);
}

void Scene::createCreditsTimeline(Timeline& timeline, std::vector<std::string> creditsPaths, int windowWidth, int windowHeight) {
	float fadeDuration = 2.0f;
	float creditsStayDuration = 3.0f;
	float baseDelay = 2.5f;

	float creditsSlotDuration = fadeDuration + creditsStayDuration + fadeDuration;

	for (int i = 0; i < creditsPaths.size(); i++) {
		std::string creditsPath = creditsPaths[i];

		auto& creditsEntity = UIUtils::createBackground(world, windowWidth, windowHeight, creditsPath.c_str());
		auto& fade = creditsEntity.addComponent<Fade>(fadeDuration);
		auto& sprite = creditsEntity.getComponent<Sprite>();
		sprite.visible = false;

		float fadeInStart = baseDelay + creditsSlotDuration * i;

		// Fade credit in.
		timeline.timeline.emplace_back(fadeInStart, [&fade, &sprite] {
			sprite.visible = true;
			fade.durationTimer = 0.0f;
			fade.isFading = true;
		});

		float fadeOutStart = fadeInStart + fadeDuration + creditsStayDuration;

		// Fade credit out in time for the next credit.
		timeline.timeline.emplace_back(fadeOutStart, [&fade] {
			fade.startingAlpha = 255;
			fade.endingAlpha = 0;
			fade.durationTimer = 0.0f;
			fade.isFading = true;
		});

		// Go back to the main menu a bit after the last credit.
		if (i == creditsPaths.size() - 1) {
			timeline.timeline.emplace_back(fadeOutStart + fadeDuration + 0.75f, [] {
				Game::onSceneChangeRequest("mainmenu");
			});
		}
	}
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
	UIUtils::createIconCounter(world, dynamicLeftPadding, (fontHeight + leading) * 2 + paddingY * 1.5, PlayerStats::MAX_HEALTH,
		fontHeight, fontHeight, IconCounterType::Health, "../asset/ui/red-star.png");

	// Bomb static and dynamic labels.
	UIUtils::createLabel(world, staticLeftPadding, (fontHeight + leading) * 3 + paddingY * 1.5, lightPink, staticLabelFont,
		"Bomb", "BombLabel", LabelType::Static);
	UIUtils::createIconCounter(world, dynamicLeftPadding, (fontHeight + leading) * 3 + paddingY * 1.5, PlayerStats::MAX_BOMBS,
		fontHeight, fontHeight, IconCounterType::Bomb, "../asset/ui/blue-star.png");

	// Power static and dynamic labels.
	UIUtils::createLabel(world, staticLeftPadding, (fontHeight + leading) * 4 + paddingY * 2, hotPink, staticLabelFont,
		"Power", "PowerLabel", LabelType::Static);
	UIUtils::createLabel(world, dynamicLeftPadding, (fontHeight + leading) * 4 + paddingY * 2, offWhite, dynamicLabelFont,
		"0", "Power", LabelType::Power);

	// Graze static and dynamic labels. Currently hidden since not implemented, kept for potential future use.
	// UIUtils::createLabel(world, staticLeftPadding, (fontHeight + leading) * 5 + paddingY * 2, hotPink, staticLabelFont,
	// 	"Graze", "GrazeLabel", LabelType::Static);
	// UIUtils::createLabel(world, dynamicLeftPadding, (fontHeight + leading) * 5 + paddingY * 2, offWhite, dynamicLabelFont,
	// 	"0", "Graze", LabelType::Graze);

	// Point static and dynamic labels.
	UIUtils::createLabel(world, staticLeftPadding, (fontHeight + leading) * 5 + paddingY * 2, hotPink, staticLabelFont,
		"Point", "PointLabel", LabelType::Static);
	UIUtils::createLabel(world, dynamicLeftPadding, (fontHeight + leading) * 5 + paddingY * 2, offWhite, dynamicLabelFont,
		"0", "Point", LabelType::Point);
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
		"Quit To Title", "PauseQuitTitleButton", [this] {
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
	UIUtils::addOverlayUIComponents(world, overlay, windowWidth, windowHeight, "Pause", "PauseLabel", buttons);

	// Add a short fade in to all the children of the overlay.
	for (auto& entity : overlay.getComponent<Children>().children) {
		entity->addComponent<Fade>(0.15f);
	}

	// Add a short fade in to the overlay.
	overlay.addComponent<Fade>(0.15f);
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
					// Update continues so far.
					playerStats.currentContinues++;
					Game::gameState.continues = playerStats.currentContinues;

					// Set player health back to 3 and reset score to 0 + current continues.
					playerStats.currentHealth = 3;
					playerStats.currentScore = playerStats.currentContinues;

					break;
				}
			}

			// Toggle overlay.
			toggleComponent.toggle();
		});

	// Create quit to title button.
	auto& quitButton =  UIUtils::createSelectableButton(world, font, selectedColour, unselectedColour,
		"Quit To Title", "ContinueQuitTitleButton", [this] {
			// Request scene change to main menu.
			Game::onSceneChangeRequest("mainmenu");
		});

	std::vector<Entity*> buttons;
	buttons.push_back(&continueButton);
	buttons.push_back(&quitButton);

	// Create the overlay with title and buttons.
	UIUtils::addOverlayUIComponents(world, overlay, windowWidth, windowHeight, "Continue? Score will be reset", "ContinueLabel", buttons);

	// Add a short fade in to all the children of the overlay.
	for (auto& entity : overlay.getComponent<Children>().children) {
		entity->addComponent<Fade>(0.15f);
	}

	// Add a short fade in to the overlay.
	overlay.addComponent<Fade>(0.15f);
}

void Scene::createWinGameMenuUComponents(Entity& overlay, int windowWidth, int windowHeight) {
	if (!overlay.hasComponent<Toggleable>()) {
		std::cerr << "Overlay must have Toggleable component!" << std::endl;
	}

	// Label colours and font.
	SDL_Color selectedColour {202, 101, 101, 255};
	SDL_Color unselectedColour {48, 55, 69, 255};
	const char* font = "pop1";

	auto& darken = world.createEntity();
	SDL_Texture *darkenTex = TextureManager::load("../asset/ui/darken.png");
	SDL_FRect src {0, 0, static_cast<float>(darkenTex->w), static_cast<float>(darkenTex->h)};
	SDL_FRect dst = StageUtils::CalculateStageRect(windowWidth, windowHeight);
	darken.addComponent<Transform>(Vector2D(dst.x, dst.y), 0.0f, 1.0f);
	darken.addComponent<Sprite>(darkenTex, src, dst, RenderLayer::UI, Vector2D(0, 0), false);

	darken.addComponent<Parent>(&overlay);
	overlay.getComponent<Children>().children.push_back(&darken);

	// Create quit to title button.
	auto& creditButton =  UIUtils::createSelectableButton(world, font, selectedColour, unselectedColour,
		"View Credits", "CreditsButton", [this] {
			// Request scene change to credits.
			Game::onSceneChangeRequest("credits");
		});

	// Create quit to title button.
	auto& quitTitleButton =  UIUtils::createSelectableButton(world, font, selectedColour, unselectedColour,
		"Quit To Title", "WinQuitTitleButton", [this] {
			// Request scene change to main menu.
			Game::onSceneChangeRequest("mainmenu");
		});

	// Create quit game button.
	auto& quitButton =  UIUtils::createSelectableButton(world, font, selectedColour, unselectedColour,
		"Quit Game", "WinQuitButton", [] {
			// Quit game by pushing a quit event.
			SDL_Event event;
			event.type = SDL_EVENT_QUIT;
			SDL_PushEvent(&event);
		});

	std::vector<Entity*> buttons;
	buttons.push_back(&creditButton);
	buttons.push_back(&quitTitleButton);
	buttons.push_back(&quitButton);

	// Create the overlay with title and buttons.
	UIUtils::addOverlayUIComponents(world, overlay, windowWidth, windowHeight, "Congratulations, You Win!", "WinLabel", buttons);

	// Add a delayed fade in to all the children of the overlay for the win game menu.
	for (auto& entity : overlay.getComponent<Children>().children) {
		auto& fade = entity->addComponent<Fade>();
		fade.fadeDelayDuration = 3.5f;

		// Make the title duration a bit shorter.
		if (entity->hasComponent<Label>()) {
			if (entity->getComponent<Label>().text == "Congratulations, You Win!") {
				fade.fadeDelayDuration = 1.0f;
			}
		}
	}

	// Add a short fade in to the overlay.
	overlay.addComponent<Fade>(0.5f);
}

void Scene::createBossHealthBar(int windowWidth, int windowHeight) {
	float stageWidth = StageUtils::CalculateStageWidth(windowWidth);

	float barWidth = stageWidth * 0.965f;
	float barPadding = (stageWidth - barWidth) / 2.0f;
	float barX = StageUtils::CalculateStagePaddingX(windowWidth) + barPadding;
	float barY = StageUtils::CalculateStagePaddingY(windowHeight) + barPadding * 0.75f;

	auto& bossHealthBarEntity = world.createEntity();
	auto& children = bossHealthBarEntity.addComponent<Children>();
	bossHealthBarEntity.addComponent<Transform>(Vector2D(barX,  barY), 0.0f, 1.0f);

	// Health bar sprite.
	SDL_Texture* tex = TextureManager::load("../asset/ui/health-bar.png");
	SDL_FRect src {0, 0, static_cast<float>(tex->w), static_cast<float>(tex->h)};
	SDL_FRect dst = {barX, barY, barWidth, 8};
	auto& sprite = bossHealthBarEntity.addComponent<Sprite>(tex, src, dst, RenderLayer::UI);
	sprite.visible = false;

	auto& bossHealthBar = bossHealthBarEntity.addComponent<BossHealthBar>(sprite.dst.w);
	bossHealthBarEntity.addComponent<Fade>(0.5f);

	// Boss name label.
	SDL_Color colour {206, 197, 237, 255};
	const char* font = "tiranti";
	float labelY = barY + 13.0f;
	auto& bossNameLabel = UIUtils::createLabel(world, barX, labelY, colour, font, " ",
		"BossLabel", LabelType::Static);
	auto& label = bossNameLabel.getComponent<Label>();
	label.outlineColor = {29, 25, 55, 200};
	label.visible = false;

	bossNameLabel.addComponent<Parent>(&bossHealthBarEntity);
	children.children.push_back(&bossNameLabel);
	bossNameLabel.addComponent<Fade>(0.5f, bossHealthBar.initializationDuration);

	// Phase icon counter.
	float iconSize = 24.0f;
	float counterY = labelY + iconSize + 3.0f;
	auto& phaseCounter = UIUtils::createIconCounter(world, barX, counterY, 3, iconSize, iconSize,
		IconCounterType::BossPhase, "../asset/ui/white-star.png");
	phaseCounter.addComponent<Parent>(&bossHealthBarEntity);
	children.children.push_back(&phaseCounter);

	phaseCounter.getComponent<IconCounter>().visible = false;
	phaseCounter.addComponent<Fade>(0.5f, bossHealthBar.initializationDuration);

	// Make the health bar toggleable.
	bossHealthBarEntity.addComponent<Toggleable>([&bossHealthBarEntity] {
		UIUtils::toggleOverlayVisibility(bossHealthBarEntity);
	});
}

void Scene::resetGameState() {
	Game::gameState.hiScore = 0;
	Game::gameState.score = 0;
	Game::gameState.playerHealth = 3;
	Game::gameState.playerBombs = 3;
	Game::gameState.power = 0;
	Game::gameState.graze = 0;
	Game::gameState.point = 0;
	Game::gameState.continues = 0;
}