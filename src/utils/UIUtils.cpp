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

Entity& UIUtils::createIconLabel(World& world, int x, int y, int maxNumber, float iconWidth, float iconHeight,
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
	overlay.addComponent<Sprite>(overlayTex, overlaySrc, overlayDest, RenderLayer::UI, false);

	// Add a toggleable component so the overlay can be toggled.
	overlay.addComponent<Toggleable>([&overlay, toggleVisibilityFunction]() {
		toggleVisibilityFunction(overlay);
	});

	// Create the specific UI components in the overlay.
	createUIComponents(overlay, windowWidth, windowHeight);

	return overlay;
}

bool UIUtils::toggleOverlayVisibility(Entity& overlay) {
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

	return newVisibility;
}