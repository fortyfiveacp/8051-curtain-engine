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

Entity& UIUtils::createSelectableButton(World& world, Entity& overlay, const char* font, SDL_Color selectedColour, SDL_Color unselectedColour,
	const char* text, const char* cacheKey, const std::function<void()>& onPressed) {
	auto& parentChildren = overlay.getComponent<Children>();

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

	// Add overlay as parent to the button.
	button.addComponent<Parent>(&overlay);
	parentChildren.children.push_back(&button);

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