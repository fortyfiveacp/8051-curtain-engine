#include "GameObject.h"

#include <iostream>
#include <ostream>

#include "manager/TextureManager.h"

GameObject::GameObject(const char *path, float x, float y) :
xPos(x), yPos(y), texture(TextureManager::load(path))
{

}

GameObject::~GameObject() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void GameObject::update(float deltaTime) {
    // Move GameObject one pixel on the x and y each frame.
    xPos += 50 * deltaTime;
    yPos += 50 * deltaTime;

    // Where do we want to crop the texture from?
    srcRect.x = srcRect.y = 0;

    // How much to crop?
    srcRect.w = srcRect.h = 32;

    // Where to draw it?
    dstRect.x = xPos;
    dstRect.y = yPos;

    // What sizes to render?
    dstRect.w = srcRect.w;
    dstRect.h = srcRect.h;
}

void GameObject::draw() {
    TextureManager::draw(texture, srcRect, dstRect);
}