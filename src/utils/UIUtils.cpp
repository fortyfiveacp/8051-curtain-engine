#include "UIUtils.h"
#include "Component.h"
#include "World.h"
#include "manager/AssetManager.h"
#include "manager/AudioManager.h"
#include "manager/TextureManager.h"

void UIUtils::updateIconCounter(Entity& entity) {
    auto& iconCounter = entity.getComponent<IconCounter>();

    if (!iconCounter.dirty) {
        return;
    }

    // The actual icons should be children of the entity.
    if (entity.hasComponent<Children>()) {
        auto& children = entity.getComponent<Children>();

        // Check if the current number is greater than the capacity of the counter.
        // Logic for ensuring the current number stays within the bound should be handled elsewhere.
        if (iconCounter.currentNumber > children.children.size()) {
            std::cerr << "Icon counter has exceeded the set maximum size!" << std::endl;
        }

        // Make the current number of icons visible, then make the rest invisible.
        for (int i = 0; i < children.children.size(); i++) {
            if (i < iconCounter.currentNumber) {
                children.children[i]->getComponent<Sprite>().visible = true;
            } else {
                children.children[i]->getComponent<Sprite>().visible = false;
            }
        }
    }
}

Entity& UIUtils::createFadeInMenuLayer(World& world, float width, float height, const char* texturePath, float fadeDuration, float fadeDelay) {
	auto& entity(world.createEntity());
	entity.addComponent<Transform>(Vector2D(0, 0), 0.0f, 1.0f);

	SDL_Texture* text = TextureManager::load(texturePath);
	SDL_FRect src {0, 0, static_cast<float>(text->w), static_cast<float>(text->h)};
	SDL_FRect dst {0, 0, width, height};
	entity.addComponent<Sprite>(text, src, dst, RenderLayer::UI);

	auto& charFade = entity.addComponent<Fade>(fadeDuration, fadeDelay);
	charFade.isFading = true;

	return entity;
}

Entity& UIUtils::createSelectableButton(World& world, const char* font, SDL_Color selectedColour, SDL_Color unselectedColour,
	const char* text, const char* cacheKey, const std::function<void()>& onPressed) {

	// Create the button.
	auto& button = createLabel(world, 0, 0, selectedColour, font,
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

	return button;
}

Entity& UIUtils::createLabel(World& world, int x, int y, SDL_Color colour, const char* fontName, const char* text, const char* cacheKey,
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

Entity& UIUtils::createIconCounter(World& world, int x, int y, int maxNumber, float iconWidth, float iconHeight,
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

Entity& UIUtils::createStageOverlay(World& world,int windowWidth, int windowHeight, const char* backgroundPath,
		const std::function<void(Entity& overlay, int windowWidth, int windowHeight)>& createUIComponents,
		const std::function<void(Entity& overlay)>& toggleVisibilityFunction) {
	auto &overlay(world.createEntity());
	// Create the background for the overlay that covers the stage.
	SDL_Texture *overlayTex = TextureManager::load(backgroundPath);
	SDL_FRect overlaySrc {0, 0, static_cast<float>(overlayTex->w), static_cast<float>(overlayTex->h)};
	SDL_FRect overlayDest = StageUtils::CalculateStageRect(windowWidth, windowHeight);
	overlay.addComponent<Transform>(Vector2D(overlayDest.x, overlayDest.y), 0.0f, 1.0f);
	overlay.addComponent<Sprite>(overlayTex, overlaySrc, overlayDest, RenderLayer::UI, Vector2D(0, 0), false);

	// Add children to overlay.
	overlay.addComponent<Children>();

	// Add a toggleable component so the overlay can be toggled.
	overlay.addComponent<Toggleable>([&overlay, toggleVisibilityFunction]() {
		toggleVisibilityFunction(overlay);
	});

	// Create the specific UI components in the overlay.
	createUIComponents(overlay, windowWidth, windowHeight);

	return overlay;
}

void UIUtils::addOverlayUIComponents(World& world, Entity& overlay, int windowWidth, int windowHeight, const char* titleText,
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
	auto& continueTitle = createLabel(world, 0, 0, titleColour, font, titleText, titleCacheKey, LabelType::Static);
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

bool UIUtils::toggleOverlayVisibility(Entity& overlay) {
	auto& sprite = overlay.getComponent<Sprite>();
	bool newVisibility = !sprite.visible;
	sprite.visible = newVisibility;

	if (overlay.hasComponent<Fade>()) {
		overlay.getComponent<Fade>().isFading = newVisibility;
	}

	if (overlay.hasComponent<Children>()) {
		auto& c = overlay.getComponent<Children>();

		for (auto& child : c.children) {
			if (child && child->hasComponent<Label>()) {
				child->getComponent<Label>().visible = newVisibility;
			}

			if (child && child->hasComponent<Sprite>()) {
				child->getComponent<Sprite>().visible = newVisibility;
			}

			if (child && child->hasComponent<Fade>()) {
				child->getComponent<Fade>().isFading = newVisibility;
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

	return newVisibility;
}